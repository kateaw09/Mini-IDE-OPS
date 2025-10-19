const express = require('express');
const fs = require('fs').promises;
const path = require('path');
const cors = require('cors');
const { exec } = require('child_process');
const os = require('os');
const { SerialPort } = require('serialport'); // [NEW] เพิ่ม Library สำหรับจัดการ Serial Port

const app = express();
const port = 3000;

// --- Define Directories ---
const srcDir = path.join(__dirname, 'src');
const codeDir = path.join(__dirname, 'src', 'ArduinoFiles'); // โฟลเดอร์สำหรับเก็บไฟล์ .ino

// --- Middleware ---
app.use(cors());
app.use(express.json());
app.use(express.static(srcDir));

// ===== Configuration (ผู้ใช้ต้องแก้ไขส่วนนี้) =====
const ARDUINO_CLI_PATH = path.join(__dirname, 'arduino-cli');
const FQBN = 'm5stack:esp32:m5stack_atom';
// const PORT = 'COM7'; // [REMOVED] เราจะลบค่า Port ที่ตายตัวนี้ออก เพราะจะรับมาจาก GUI แทน
// =================================================

// --- API Endpoints ---

// [NEW] Endpoint ใหม่สำหรับดึงรายชื่อ Port ที่เชื่อมต่ออยู่
app.get('/api/ports', async (req, res) => {
    try {
        const ports = await SerialPort.list();
        // ส่งเฉพาะ path ของ port กลับไป เช่น ['COM7', '/dev/ttyUSB0']
        res.json(ports.map(port => port.path));
    } catch (err) {
        console.error("Error listing serial ports:", err);
        res.status(500).json({ error: 'Could not list serial ports.' });
    }
});

app.get('/', (req, res) => {
    res.sendFile(path.join(srcDir, 'index.html'));
});

app.get('/files', async (req, res) => {
    try {
        const files = await fs.readdir(codeDir);
        const inoFiles = files.filter(file => path.extname(file).toLowerCase() === '.ino');
        res.json(inoFiles);
    } catch (err) {
        console.error("Error reading code directory:", err);
        res.status(500).json({ error: "Could not read code directory" });
    }
});

app.get('/files/:filename', async (req, res) => {
    const filename = req.params.filename;
    if (filename.includes('..') || filename.includes('/')) {
        return res.status(400).send("Invalid filename.");
    }
    const filePath = path.join(codeDir, filename);
    try {
        const data = await fs.readFile(filePath, 'utf8');
        res.type('text/plain').send(data);
    } catch (err) {
        res.status(404).send("File not found.");
    }
});

app.post('/files/:filename', async (req, res) => {
    const filename = req.params.filename;
    if (filename.includes('..') || filename.includes('/')) {
        return res.status(400).send("Invalid filename.");
    }
    const { content } = req.body;
    if (typeof content !== 'string') {
        return res.status(400).json({ error: "Invalid request body. Expecting { \"content\": \"...\" }" });
    }
    const filePath = path.join(codeDir, filename);
    try {
        await fs.writeFile(filePath, content, 'utf8');
        res.json({ message: `File '${filename}' saved successfully.` });
    } catch (err) {
        res.status(500).json({ error: `Could not save file ${filename}` });
    }
});

// [MODIFIED] แก้ไข Endpoint สำหรับ Upload Code ให้รับค่า port จาก request body
app.post('/upload', async (req, res) => {
    // 1. รับชื่อไฟล์หลักและ port จาก JSON body
    const { filename: mainSketchFile, port: selectedPort } = req.body; // <--- รับ port เพิ่ม

    if (!mainSketchFile || !mainSketchFile.endsWith('.ino')) {
        return res.status(400).json({ success: false, message: 'Invalid or missing sketch filename.' });
    }
    // เพิ่มการตรวจสอบ port
    if (!selectedPort) {
        return res.status(400).json({ success: false, message: 'Port not selected.' });
    }

    let tempSketchDir;
    try {
        const tempDirPrefix = path.join(os.tmpdir(), 'arduino-sketch-');
        tempSketchDir = await fs.mkdtemp(tempDirPrefix);
        const tempSketchFileName = path.basename(tempSketchDir) + '.ino';
        const tempSketchPath = path.join(tempSketchDir, tempSketchFileName);

        const allFiles = await fs.readdir(codeDir);
        const inoFiles = allFiles.filter(file => file.endsWith('.ino'));
        const otherInoFiles = inoFiles.filter(f => f !== mainSketchFile);

        let combinedCode = '';
        for (const file of otherInoFiles) {
            const content = await fs.readFile(path.join(codeDir, file), 'utf8');
            combinedCode += content + '\n\n';
        }
        const mainContent = await fs.readFile(path.join(codeDir, mainSketchFile), 'utf8');
        combinedCode += mainContent;

        await fs.writeFile(tempSketchPath, combinedCode, 'utf8');

        console.log('--- Starting Arduino CLI Upload ---');
        console.log(`Board: ${FQBN}`);
        console.log(`Port: ${selectedPort}`); // <--- ใช้ port ที่รับมา
        console.log(`Temporary Sketch Path: ${tempSketchDir}`);

        // 7. สั่ง compile และ upload โดยใช้ port ที่ผู้ใช้เลือก
        const command = `${ARDUINO_CLI_PATH} compile --upload -p ${selectedPort} --fqbn ${FQBN} "${tempSketchDir}"`;

        const execPromise = new Promise((resolve, reject) => {
            exec(command, (error, stdout, stderr) => {
                if (error) {
                    console.error(`Exec error: ${error.message}`);
                    reject({ success: false, message: `Compilation/Upload Failed:\n${stderr}\n${stdout}` });
                    return;
                }
                resolve({ success: true, message: `Upload successful!\n${stdout}` });
            });
        });

        const result = await execPromise;
        res.json(result);
    } catch (error) {
        console.error("An error occurred during the upload process:", error);
        res.status(500).json(error.message ? { success: false, message: error.message } : error);
    } finally {
        if (tempSketchDir) {
            await fs.rm(tempSketchDir, { recursive: true, force: true });
            console.log(`Cleaned up temporary directory: ${tempSketchDir}`);
        }
    }
});

// --- Start the server ---
app.listen(port, () => {
    console.log("--- Simple IDE Server (Node.js) ---");
    console.log(`- Server is ready at: http://localhost:${port}`);
    console.log("-------------------------------------");
});