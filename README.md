# XLSX-TXT

## [Install MinGW](https://code.visualstudio.com/docs/languages/cpp#_example-install-mingwx64-on-windows)
-  Baixeu-lo mitjançant aquest enllaç directe a [l'instal·lador de MinGW](https://github.com/msys2/msys2-installer/releases/download/2023-05-26/msys2-x86_64-20230526.exe).
-  Executeu l'instal·lador i seguiu els passos de l'assistent d'instal·lació. Tingueu en compte que MSYS2 requereix Windows 8.1 de 64 bits o posterior.
-  A l'assistent, trieu la carpeta d'instal·lació que vulgueu. Guardeu aquest directori per a més endavant. En la majoria dels casos, el directori recomanat és acceptable. El mateix s'aplica quan arribeu a configurar el pas de dreceres del menú d'inici. Quan s'hagi completat, assegureu-vos que la casella Executar MSYS2 ara estigui marcada i seleccioneu Finalitzar. Aleshores s'obrirà automàticament una finestra de terminal MSYS2.
-  En aquest terminal, instal·leu la cadena d'eines MinGW-w64 executant l'ordre següent:
```pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain```
-  Accepteu el nombre predeterminat de paquets del grup d'eines prement Retorn.
-  Introduïu Y quan se us demani si voleu continuar amb la instal·lació.
-  Afegiu el camí a la carpeta bin MinGW-w64 a la variable d'entorn PATH de Windows seguint els passos següents:
   1. A la barra de cerca de Windows, escriviu Configuració per obrir la configuració de Windows.  
   2. Cerqueu Edita variables d'entorn per al vostre compte.  
   3. A les variables d'usuari, seleccioneu la variable Camí i, a continuació, seleccioneu Edita.  
   4. Seleccioneu Nou i afegiu la carpeta de destinació MinGW-w64 que heu enregistrat durant el procés d'instal·lació a la llista. Si heu seleccionat els passos d'instal·lació per defecte, el camí és: C:\msys64\ucrt64\bin.  
   5. Seleccioneu D'acord per desar el PATH actualitzat. Perquè el nou PATH estigui disponible, torneu a obrir les finestres de la vostra consola.  
- Comproveu que les vostres eines MinGW-w64 estiguin instal·lades i disponibles correctament, obriu un nou símbol del sistema i escriviu:
```
gcc --version
g++ --version
gdb --version
```
## Compile:
Utilitzeu la següent comanda: 
```g++ -std=c++17 main.cpp -o main -lcomdlg32```
