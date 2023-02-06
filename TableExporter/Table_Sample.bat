@echo off
setlocal EnableDelayedExpansion

:: 실행
cd /d F:\Program\Tool\Exporter\Table\bin\Debug
start /wait Table.exe export_tbd export_tbd_and_code export_patchlist

pause
