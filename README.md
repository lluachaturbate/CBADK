# CBADK
App Dev Kit for Chaturbate


#### Why i did this?
While the testbed is a good thing to do design and final tests it's not really handy while developing apps.

Especially when people aren't writing javascript code on a daily basis they make a lot of mistakes or want to test every new little piece of code. With the amount of time it takes to upload and start apps on the testbed you end up spending less than half of the time on coding.



#### Take into consideration
* This is not made to be visually compatible with chaturbate. I tried to make it look close, that's it.
  * The font slider is there for a reason tho: Broadcasters usually increase the fontsize by a lot - think of that when spamming notices.
* I coded this on a day off quickly, **dirty**, without a design idea and without testing - educated guess: There is a bug somewhere.
* There is no guarantee that anything that works here will work on chaturbate. All i did is based on the API documentation and guesswork. Always do your final tests on the testbed!



#### Make it better
Don't hesitate to report bugs and make feature/pull requests. I originally did this for myself and didn't intend to fit the needs of everyone.



##### TODO
I usually don't make GUIs so contribution from someone fluent with QML would be nice. Don't hesitate to make a rework.
Doxygen and my QML mess should give you enough info to do it.


##### Install
It only depends on [Qt](https://www.qt.io/) so all that is needed is
```
qmake && make
```
Since Windows-people usually don't like to compile things i put up a release file. It's just **blindly** cross compiled with MXE and never tested. If it doesn't work just get Qt and compile it yourself, it's maybe 10clicks and a coffee.


##### Usage
It should be pretty forward,:
* Load your app code.
  * since there is no server the combobox at the top controls *your view* of the chat and cam.
  * create viewers and events (chat, tip, enter, leave) with the controls in the lower right.
