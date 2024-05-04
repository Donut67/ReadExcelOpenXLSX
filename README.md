# XLSX-TXT

# Install MinGW: https://code.visualstudio.com/docs/languages/cpp#_example-install-mingwx64-on-windows
1. Download using this direct link to the MinGW installer.
2. Run the installer and follow the steps of the installation wizard. Note, MSYS2 requires 64 bit Windows 8.1 or newer.
3. In the wizard, choose your desired Installation Folder. Record this directory for later. In most cases, the recommended directory is acceptable. The same applies when you get to setting the start menu shortcuts step. When complete, ensure the Run MSYS2 now box is checked and select Finish. A MSYS2 terminal window will then automatically open.
4. In this terminal, install the MinGW-w64 toolchain by running the following command:
  pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
5. Accept the default number of packages in the toolchain group by pressing Enter.
6. Enter Y when prompted whether to proceed with the installation.
7. Add the path to your MinGW-w64 bin folder to the Windows PATH environment variable by using the following steps:
  1. In the Windows search bar, type Settings to open your Windows Settings.
  2. Search for Edit environment variables for your account.
  3. In your User variables, select the Path variable and then select Edit.
  4. Select New and add the MinGW-w64 destination folder you recorded during the installation process to the list. If you selected the default installation steps, the path is: C:\msys64\ucrt64\bin.
  5. Select OK to save the updated PATH. For the new PATH to be available, reopen your console windows.
8. Check that your MinGW-w64 tools are correctly installed and available, open a new Command Prompt and type:
  gcc --version
  g++ --version
  gdb --version
#Compile:
Use the next command: g++ -std=c++17 main.cpp -o main -lcomdlg32
