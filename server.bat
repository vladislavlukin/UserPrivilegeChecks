@echo off
call node-gyp configure build
start /B cmd /c "node server.js"
timeout 5 >nul
start http://localhost:8080
