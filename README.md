# Sub-Etha-Tinycomms

## Overview

**Sub-Etha-Tinycomms** is an toy project using some comms and peripherals
targeting an ATtiny214 chip. Thus far there have been a number of rabbit holes I
elected to explore. Took longer than anticipated, but well worth the time.

## Project Activities

- Setup docker based build environment that includes:
  - avr-gcc build environment for ATTiny214 processor
  - cpputest environment for unit testing
  - Docker Hub hosted image, `mattprompt/sub-etha-tinycomms:latest`
    https://hub.docker.com/repository/docker/mattprompt/sub-etha-tinycomms
  - Dockerfile on Github under sub-etha-tinycomms/docker
- Github [repository](https://github.com/mattprompt/sub-etha-tinycomms)
- Github Actions for automated running of unit tests and builds on commit, which
  saves build artifacts
- Development of comms that was actually requested
- Installed Microchip Studio on Windows as a check environment and to look at
  example projects for configuring peripherals

## Potential Improvements

- Message check bit or CRC, especially if none is provided by other layers in
  the protocol stack
- Define error message so if an unknown command is received, the uC can respond
  and not fail silently
- Start Character / End Character

## avr-gcc setup

Setting up avr-gcc was a bit of a pain, because the ATtiny214 device is not
supported. At least not with the version in the Ubuntu Apt package manager. I
opted to tweak/patch the install based on the Atmel ATtiny_DFP pack. But
building the latest avr-gcc and associated libraries from scratch may be a
better long term solution.

## Cpputest setup

Installing Cpputest was mostly based on instructions from the Cpputest docs.

## Standards & Style

- Conventional Commits for commit messages
  [https://www.conventionalcommits.org/en/v1.0.0/]
- Gitflow for Git workflow
  [https://nvie.com/posts/a-successful-git-branching-model/]
  [https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow]
- Github pull request workflow for merging [probably won't do, already taking
  too long]
- 4 spaces for indent, no tabs
- 80 character line width

## Other stuff I won't get to

- Jira setup [probably not going to happen, minimal value add for toy project]
- Doxygen build [probably not, project already taking too long]

```
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
```
