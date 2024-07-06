@echo off

rem 设置源目录和目标目录路径
set "source1=LYGui\bin\x64\Release"
set "source2=LYGui\LYGui\Asset"
set "destination=D:\Billy\MyApplication\LYGUI"

rem 执行复制操作
echo 复制 %source1% 到 %destination%
xcopy /E /I /Y "%source1%" "%destination%\%source1%"

echo 复制 %source2% 到 %destination%
xcopy /E /I /Y "%source2%" "%destination%\%source2%"

echo 复制完成！
pause
