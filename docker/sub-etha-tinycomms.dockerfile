FROM ubuntu:latest

WORKDIR /workspace
ADD io.h.patch .

# Install AVR environment 
# ref: https://www.linuxandubuntu.com/home/setting-up-avr-gcc-toolchain-and-avrdude-to-program-an-avr-development-board-in-ubuntu
# Note installation order is important
RUN apt-get update && apt-get upgrade -y --no-install-recommends && \
    apt-get install -y --no-install-recommends \
    binutils \
    gcc-avr \
    avr-libc \
    uisp \
    avrdude \
    flex \
    byacc \
    bison \
    file \
    gcc \
    g++ \
    make \
    automake \
    autoconf \
    libtool \
    git \
    unzip \
    wget \
    patch \
    ca-certificates

# Install CPPUTEST
# ref: https://cpputest.github.io/
#apt-get install -y cpputest
# Need to wait for git clone to finish
RUN git clone -v --progress git://github.com/cpputest/cpputest.git \
    && echo "Clone Finished" \
    && cd /workspace/cpputest \
    && autoreconf . -i && ./configure && make tdd

ENV CPPUTEST_HOME=/workspace/cpputest

# Patch avr-gcc environment to work with AtTiny214
WORKDIR /workspace/ATtiny_DFP
RUN wget http://packs.download.atmel.com/Atmel.ATtiny_DFP.1.9.337.atpack \
    && unzip Atmel.ATtiny_DFP.1.9.337.atpack -d /workspace/ATtiny_DFP \
    && cp ./gcc/dev/attiny214/avrxmega3/short-calls/crtattiny214.o /lib/avr/lib/avrxmega3/short-calls/ \
    && cp ./gcc/dev/attiny214/avrxmega3/short-calls/libattiny214.a /lib/avr/lib/avrxmega3/short-calls/ \
    && cp ./include/avr/iotn214.h /lib/avr/include/avr \
    && patch -b /lib/avr/include/avr/io.h -i /workspace/io.h.patch

CMD /bin/bash

#MMMMMMMMMMMMMMMMMMMMMMMMMMMNKkdl:,...         ...';cok0NWMMMMMMMMMMMMMMMMMMMMMM
#WXOdooodxk0XNWMMMMMMMMMN0d:'.                         .':oOXWMMMMMMMMMMMMMMMMMM
#d'        ..,:okKNMWXkl'.                                  .:xKWMMMMMMMMMMMMMMM
#.    ;odddol:'...'::'                                         .ckNMMMMMMMMMMMMM
#.   ;XMMMMMMWNKd'                                                ,xXWMMMMMMMMMM
#,   ;XMMMMMMMNO;                                          .,,.     'xNMMMMMMMMM
#x.  .dWMMMMWKl.                                        .;xXNNKd'     ;OWMMMMMMM
#Nd.  .kWMMWk'      .;loddoc,.                        .lONMMMMMWXl.    .dNMMMMMM
#MNd.  .xNNo.    .:xXWMMMMMMN0o'                    ,dKWWMMMMMMMM0'     .cXMMMMM
#MMWx.  .cc.     c0XNNWMMMMMMMMXk;.               ;xXMMMMMWWMMMW0c..      cXMMMM
#MMMWO,  :o.     ..''';cd0NMMMMMMNO:.            :XMMMMMMMWMMNOdox00kl.    cXMMM
#MMMMMKc:0W0:.           .:kNMMMMMMWO:.          ;KWMMMMMMWXkdx0WMMMMW0:.  .oNMM
#MMMMMM0cxNMNx'            .:OWMMMMMMNkc,'.       ,kNWMMWKxdkXWMMMMMMMMK;   'OMM
#MMMMMWo..:0WWKl.            .kWMMMMMMMWNX0kdc'.   .,looodONWMMMMMMMMMNk.    lNM
#MMMMMX;   .oXMW0:.           ,0MMMMMMMMMMMMMWNOo'     .kWMMMMMMMMMWNk:.     '0M
#MMMMMO.     ,kNMNk;           ;0WMMMMMMMMMMMMMMMXx,   ,KMMMMMMMMWKd,        .xM
#MMMMMk.      .:OWMNx,          'kNMMMMMMMMMMMMMMMMXo.  ;OWMMMWW0dllol;.      oW
#MMMMWx.        .c0WMXx,         .lKMMMMMMMMMMMMMMMMWk'  .cxOOxooONMMMNO;     oW
#MMMMMk.          .lKWMXx,         'kNMMMMMMMMMMMMMMMWO'     .l0WMMMMMMWXc   .dW
#MMMMM0'            .oKWMNk:.       .oNMMMMMMMMMMMMMMMWx.    cNMMMMMMMMWXc   .kW
#MMMMMX:              .oKWWNO:.      .kMMMMMMMMMMMMMMMMX:    ;0WMMMMMMXx,    ,0M
#MMMMMWx.               .c0WMW0l.    ,0MMMMMMMMMMMMMMMMWx.    'dKWMWKo,      oWM
#MMMMMMX:                 .cONMWKd,.;0WMMMMMMMMMMMMMMMMMO.      .;:;.       ,0MM
#MMMMMMMO'                  .;xXMMNKNMMMMMMMMMMMMMMMMMMMk.                 .xWMM
#MMMMMMMWx.                   .cXMMMMMMMMMMMMMMMMMMMMMMMx.                .oNMMM
#MMMMMMMMWx.                 .l0WMMMMMMMMMMMMMMMMMMMMMMWl                 cXMMMM
#MMMMMMMMMWk'              'dKWMMMMMMMMMMMMMMMMMMMMMMMM0,                 ,OWMMM
#MMMMMMMMMMW0;           ,xXMMMMMMMMMMMMMMMMMMMMMMMMMMXc               'l:..kNMM
#MMMMMMMMMMMMXd.      .:kNMMMMMMMMMMMMMMMMMMMMMMMMMMMXl.             .cKWNo..dNM
#MMMMMMMMMMMMMWKl.  .cONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:.           .;ONMMMNx..dN
#MMMMMMMMMMMMMMMW0odKWMMMMMMMMMMMMMMMMMMMMMMMMMMMWXXWWNOo;.      .:ONWMMMMMWd..x
#MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKo'.:d0NMWKkl'..lONMMMMMMMMMX: '
#MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKo.     .,lkKKx;.'lx0NWMMMMMMMNc  
#MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKl.       .':d0Ol,.   .':ldkOOOkc.  
#MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNk:..',;cldkKXWMMMMNKko:'.           ;
#MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNK00KXNWMMMMMMMMMMMMMMMWX0dc,..   .'oK