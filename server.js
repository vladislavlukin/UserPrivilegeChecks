const http = require('http');
const fs = require('fs');
const url = require('url');
const querystring = require('querystring');
const { checkPrivilege, listUsers } = require('./build/Release/addon');


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
                const querystring = require('querystring');

                const postData = querystring.parse(body);
                const username = postData.username.trim();
                const privilege = checkPrivilege();

                if (privilege === "error") {
                    res.writeHead(200, { 'Content-Type': 'text/html' });
                    res.write("Ошибка в получении пользователя");
                    return res.end();
                }

                let numPrivilege = " - нет такого пользователя";

                const result = privilege.split(" ");
                for (const string of result) {
                    if (string.includes(username)) {
                        const pr = string.replace(/\x00/g, '').split(":");
                        if (pr[0] === username) {
                            if (pr[1] === "0") {
                                numPrivilege = " - имеет привилегию Гостя";
                            } else if (pr[1] === "1") {
                                numPrivilege = " - имеет привилегию Пользователя";
                            } else if (pr[1] === "2") {
                                numPrivilege = " - имеет привилегию Администратора";
                            } else {
                                numPrivilege = " - не имеет привелегий";
                            }
                        }
                    }
                }

                res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
                res.write(username + numPrivilege);
                res.write(`<br><br><button onclick="history.back()">Назад</button>`);
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