pause
set starttime=Start %date% - %time%
set PatchVertion=0.0.1
Test 
"C:\Program Files\Epic Games\UE_4.27\Engine\Binaries\DotNET\AutomationTool.exe" -ScriptsForProject=F:/TestProjects/UE4_PakTest/UE4_PakTest.uproject BuildCookRun -project=F:/TestProjects/UE4_PakTest/UE4_PakTest.uproject -noP4 -clientconfig=Shipping -serverconfig=Shipping -nocompileeditor -installed -utf8output -platform=Android -targetplatform=Android -cookflavor=ASTC -build -cook -map= -unversionedcookedcontent -additionalcookeroptions=-distribution -pak -createreleaseversion=%PatchVertion% -manifests -createchunkinstall -chunkinstalldirectory=F:/TestProjects/UE4_PakTest/HttpChunkInstall/ -chunkinstallversion=%PatchVertion% -compressed -stage -package -cmdline=" -Messaging" -addcmdline="-SessionId=89A6FF3E40FFC13974B82DA19BF0F05E -SessionOwner='yklee' -SessionName='PakTest ChunkDownloader' -distribution" -distribution
set endtime=End %date% - %time%
@echo ---------- %starttime% ----------
@echo ---------- %endtime% ----------
pause