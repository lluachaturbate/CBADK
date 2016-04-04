# CBADK
App Dev Kit for Chaturbate


#### What is this?
Well the name says it all: This tool is supposed to help develop and test Apps for CB.
I decided to not attempt to make a website copy of CB and build features around it, instead i did my own implementation to realize feautures that i think are important.
Unfortunately the CB API didn't gave me much to work with, so i filled the gaps with tests and guesswork.

#### Who is this for?
Originally for me since the old ADK linked in the CB API is outdated and misses a lot of feautures.
I make barely everything i code open source so others can profit from it.
Doesn't matter if you are a beginner or a pro, you should be able to work fine with this.


#### Feautures
* All API functions.
* Multi-User support.
  * You can create users and get the POV of the user you select (notices, cam etc.)  
  * You can act as any user at any time (chat, tip, enter/leave room etc.)
* Debugger
* Easy chat fontsize adjustment
  * As a broadcaster i can tell you a lot of us **greatly** increase the fontsize since we usually sit between 1m and 5m away from the screen.
  * Notice-spamming or real big notices fill up the whole chat window disturbingly often, so you should think of (and test) that while developing.
* Image support (without animations)
  * ":whatever" will load "Images/whatever.(jpg/png/gif)"
* Code warnings.
  * Let's face it, the CB appengine has a few flaws. I attempt to give warnings if your code does something stupid.



#### Make it better
Don't hesitate to report bugs, make feature/pull requests or [mail me](mailto:llua.chaturbate@gmail.com). I originally did this for myself and didn't intend to fit the needs of everyone.


#### Compile
It is completely portable and only depends on [Qt](https://www.qt.io/) to build.
```
qmake && make
```
 * Windows 
  * Since Windows-people usually don't like to compile things i put up a release file. It's just **blindly** cross compiled with MXE and never tested. If it doesn't work just get Qt and compile it yourself, it's maybe 10clicks and a coffee. There is no knowledge of C++ required to complie and run a project (a chimpanse can do it).
