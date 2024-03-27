const http = require('http');
const fs = require('fs');
const url = require('url');
const querystring = require('querystring');
const { checkPrivilege } = require('./build/Release/addon');

const server = http.createServer((req, res) => {
    const q = url.parse(req.url, true);
    const filename = '.' + q.pathname;

    if (filename === './') {
        fs.readFile('./index.html', (err, data) => {
            if (err) {
                res.writeHead(404, { 'Content-Type': 'text/html' });
                return res.end('404 Not Found');
            }
            res.writeHead(200, { 'Content-Type': 'text/html' });
            res.write(data);
            return res.end();
        });
    } else if (filename === './check') {
        if (req.method === 'POST') {
            let body = '';
            req.on('data', (chunk) => {
                body += chunk.toString();
            });
            req.on('end', () => {
                const postData = querystring.parse(body);
                const username = postData.username;
                const privilege = checkPrivilege(username);
                res.writeHead(200, { 'Content-Type': 'text/html' });
                res.write(privilege);
                return res.end();
            });
        }
    } else {
        fs.readFile(filename, (err, data) => {
            if (err) {
                res.writeHead(404, { 'Content-Type': 'text/html' });
                return res.end('404 Not Found');
            }
            res.writeHead(200, { 'Content-Type': 'text/html' });
            res.write(data);
            return res.end();
        });
    }
});

server.listen(8080, () => {
    console.log('Server running at http://localhost:8080/');
});