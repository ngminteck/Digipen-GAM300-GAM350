echo Generating exe to use...

@set FILENAME=GENERATE_SCRIPT

@rem g++ -std=c++11 %FILENAME%.cpp -o %FILENAME%.exe
@call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
@cl %FILENAME%.cpp /FeGENERATE_SCRIPT.exe /EHsc /Za
@echo.
@CALL %FILENAME%.exe %1%
@rm %FILENAME%.exe
@rm %FILENAME%.obj
@pause