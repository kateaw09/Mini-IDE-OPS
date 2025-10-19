const express = require('express');
const fs = require('fs');
const path = require('path');
const cors = require('cors'); // Import cors middleware

// Initialize the Express application
const app = express();
const port = 3000;

// --- Middleware ---
app.use(cors()); // Enable Cross-Origin Resource Sharing
app.use(express.text()); // Middleware to parse plain text body
app.use(express.static(__dirname)); // Serve static files like ide.html


// --- API Endpoints ---

// Endpoint to get the list of .ino files
app.get('/files', (req, res) => {
    fs.readdir(__dirname, (err, files) => {
        if (err) {
            console.error("Error reading directory:", err);
            return res.status(500).json({ error: "Could not read directory" });
        }
        const inoFiles = files.filter(file => path.extname(file).toLowerCase() === '.ino');
        res.json(inoFiles);
    });
});

// Endpoint to get the content of a specific file
app.get('/files/:filename', (req, res) => {
    const filename = req.params.filename;
    // Security check to prevent directory traversal
    if (filename.includes('..') || filename.includes('/')) {
        return res.status(400).send("Invalid filename.");
    }
    const filePath = path.join(__dirname, filename);

    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            console.error(`Error reading file ${filename}:`, err);
            return res.status(404).send("File not found.");
        }
        res.type('text/plain').send(data);
    });
});

// Endpoint to save (overwrite) a file
app.post('/files/:filename', (req, res) => {
    const filename = req.params.filename;
    // Security check
    if (filename.includes('..') || filename.includes('/')) {
        return res.status(400).send("Invalid filename.");
    }
    const filePath = path.join(__dirname, filename);
    const content = req.body;

    fs.writeFile(filePath, content, 'utf8', (err) => {
        if (err) {
            console.error(`Error saving file ${filename}:`, err);
            return res.status(500).json({ error: `Could not save file ${filename}` });
        }
        console.log(`File '${filename}' was successfully saved.`);
        res.json({ message: `File '${filename}' saved successfully.` });
    });
});


// --- Start the server ---
app.listen(port, () => {
    console.log("--- Simple IDE Server (Node.js) ---");
    console.log("Starting server...");
    console.log("1. Make sure this script, ide.html, and all .ino files are in the same folder.");
    console.log(`2. Open your web browser and go to: http://localhost:${port}`);
    console.log("-------------------------------------");
});

