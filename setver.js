const fs = require('fs');
const path = require('path');
const process = require('process');

if(process.argv.length < 3) {
    console.log("Missing new version");
    return;
}

const newVer = process.argv[2];
// File paths
const configFilePath = path.join(__dirname, 'src', 'config.h');
const workflowFilePath = path.join(__dirname, '.github', 'workflows', 'build-and-release.yml');

// Function to replace a specific line in a file, accounting for spaces
function replaceLineInFile(filePath, searchLineStart, newLineContent) {
    const fileContent = fs.readFileSync(filePath, 'utf8');
    const updatedContent = fileContent
        .split('\n')
        .map(line => line.trimStart().startsWith(searchLineStart.trimStart()) ? 
            line.replace(/.*/, `${line.match(/^\s*/)[0]}${newLineContent}`) : 
            line)
        .join('\n');
    fs.writeFileSync(filePath, updatedContent, 'utf8');
    console.log(`Updated file: ${filePath}`);
}

// Update "src/config.h"
const configSearch = '#define MKVE_VERSION';
const configNewLine = `#define MKVE_VERSION "${newVer}"`;
replaceLineInFile(configFilePath, configSearch, configNewLine);

// Update ".github/workflows/build-and-release.yml"
const workflowSearch = 'echo "VERSION=';
const workflowNewLine = `echo "VERSION=v${newVer}" >> $GITHUB_ENV`;
replaceLineInFile(workflowFilePath, workflowSearch, workflowNewLine);

console.log(`Updated to version: ${newVer}`);