const express = require('express');
const fs = require('fs').promises;
const fsSync = require('fs');
const path = require('path');
const cors = require('cors');
const { exec } = require('child_process');
const os = require('os');
const { SerialPort } = require('serialport');
const multer = require('multer');
const archiver = require('archiver');

const app = express();
const port = 3000;

const srcDir = path.join(__dirname, 'src');

// สร้างไดเรกทอรีที่เขียนได้ในโฟลเดอร์ home ของผู้ใช้
const userDataDir = path.join(os.homedir(), 'Mini_IDE_Files'); 
const codeDir = path.join(userDataDir, 'ArduinoFiles');

if (!fsSync.existsSync(codeDir)) {
    fsSync.mkdirSync(codeDir, { recursive: true });
}
// --- จบส่วนที่แก้ไข ---

app.use(cors());
app.use(express.json());
app.use(express.static(__dirname)); 
app.use(express.static(srcDir));

const ARDUINO_CLI_PATH = path.join(__dirname, 'arduino-cli');
const FQBN = 'pt-robotics:esp32:PTBOT_ESP32';

const storage = multer.diskStorage({
    destination: (req, file, cb) => cb(null, codeDir),
    filename: (req, file, cb) => cb(null, file.originalname)
});

const upload = multer({
    storage: storage,
    fileFilter: (req, file, cb) => {
        if (!file.originalname.match(/\.ino$/i)) {
            return cb(new Error('Only .ino files are allowed!'), false);
        }
        cb(null, true);
    }
}).single('inoFile');

// --- API Endpoints ---

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'src', 'index.html'));
});

app.get('/api/ports', async (req, res) => {
    try {
        const ports = await SerialPort.list();
        res.json(ports.map(port => port.path));
    } catch (err) {
        res.status(500).json({ success: false, message: 'Failed to list serial ports' });
    }
});

app.get('/files', async (req, res) => {
    try {
        const files = await fs.readdir(codeDir);
        const inoFiles = files.filter(file => file.endsWith('.ino'));
        res.json(inoFiles);
    } catch (error) {
        res.status(500).send('Error reading files');
    }
});

app.get('/files/download-all', async (req, res) => {
    const archive = archiver('zip', {
        zlib: { level: 9 }
    });

    archive.on('warning', (err) => {
        if (err.code === 'ENOENT') {
            console.warn(err);
        } else {
            throw err;
        }
    });

    archive.on('error', (err) => {
        throw err;
    });

    res.attachment('ArduinoFiles.zip');
    archive.pipe(res);

    try {
        const files = await fs.readdir(codeDir);
        const inoFiles = files.filter(file => file.endsWith('.ino'));
        
        if (inoFiles.length === 0) {
            archive.finalize();
            return;
        }

        for (const file of inoFiles) {
            const filePath = path.join(codeDir, file);
            archive.file(filePath, { name: file });
        }
        
        archive.finalize();
    } catch (error) {
        console.error('Error creating zip file:', error);
        res.status(500).send('Error creating zip file');
    }
});


app.get('/files/:filename', async (req, res) => {
    try {
        const filePath = path.join(codeDir, req.params.filename);
        const content = await fs.readFile(filePath, 'utf8');
        res.send(content);
    } catch (error) {
        res.status(404).send(`File not found: ${req.params.filename}`);
    }
});

app.post('/files/:filename', async (req, res) => {
    try {
        const filePath = path.join(codeDir, req.params.filename);
        await fs.writeFile(filePath, req.body.content || '', 'utf8');
        res.json({ message: `File '${req.params.filename}' saved successfully.` });
    } catch (error) {
        res.status(500).json({ error: `Could not save file ${req.params.filename}.` });
    }
});

app.delete('/files/all', async (req, res) => {
    try {
        const files = await fs.readdir(codeDir);
        const inoFiles = files.filter(file => file.endsWith('.ino'));
        
        for (const file of inoFiles) {
            await fs.unlink(path.join(codeDir, file));
        }
        
        console.log(`Deleted ${inoFiles.length} files.`);
        res.json({ success: true, message: `Successfully deleted ${inoFiles.length} files.` });

    } catch (error) {
        console.error('Error deleting all files:', error);
        res.status(500).json({ success: false, message: 'Could not delete files.' });
    }
});

app.post('/api/upload-file', (req, res) => {
    upload(req, res, (err) => {
        if (err) {
            return res.status(400).json({ success: false, message: err.message });
        }
        if (!req.file) {
            return res.status(400).json({ success: false, message: 'No file was uploaded.' });
        }
        res.json({ success: true, message: `File '${req.file.originalname}' uploaded successfully!` });
    });
});

app.post('/upload', (req, res) => {
    const { filename, port: selectedPort } = req.body;
    let tempSketchDir = '';

    if (!filename || !selectedPort) {
        return res.status(400).json({ success: false, message: 'Filename and Port are required.' });
    }
    
    fs.mkdtemp(path.join(os.tmpdir(), `${path.basename(filename, '.ino')}-`))
        .then(folder => {
            tempSketchDir = folder;
            const sourceFilePath = path.join(codeDir, filename); // คัดลอกจาก codeDir
            const tempFilePath = path.join(tempSketchDir, filename);
            return fs.copyFile(sourceFilePath, tempFilePath);
        })
        .then(() => {
            console.log(`Starting upload for: ${filename} to ${selectedPort}`);
            const command = `"${ARDUINO_CLI_PATH}" upload -p ${selectedPort} --fqbn ${FQBN} "${tempSketchDir}" --verbose`;
            
            return new Promise((resolve, reject) => {
                exec(command, (error, stdout, stderr) => {
                    if (error) {
                        return reject({ success: false, message: `Upload Failed:\n${stderr}\n${stdout}` });
                    }
                    resolve({ success: true, message: `Upload successful!\n${stdout}` });
                });
            });
        })
        .then(result => {
            res.json(result);
        })
        .catch(error => {
            res.status(500).json(error.message ? { success: false, message: error.message } : error);
        })
        .finally(() => {
            if (tempSketchDir) {
                fs.rm(tempSketchDir, { recursive: true, force: true })
                  .then(() => console.log(`Cleaned up temporary directory: ${tempSketchDir}`))
                  .catch(err => console.error(`Failed to cleanup temp dir: ${err}`));
            }
        });
});

app.listen(port, () => {
    console.log(`--- Mini IDE Server (Node.js)[©Kateaw 2025] ---`);
    console.log(`- Server is ready at: http://localhost:${port}`);
    console.log(`- Arduino sketches folder: ${codeDir}`);
    console.log(`-------------------------------------`);
});