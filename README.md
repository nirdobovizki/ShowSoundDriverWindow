# ShowSoundDriverWindow
Make the annoying Realtek sound driver window that asks what you plug in and always gets lost below other windows the top most window

When you plug your headphones into your computer a window pops up and ask what kind of device you plugged in, but that window 
always pops up below some other window so you don't see it, and then when you start playing audio it plays using the speakers
and not the headphones.

So, I've been asking myself, that software is written by Realtek, but they are an hardware company and harware companies
are notorious for bad software UX, so I don't have high hopes they will fix it - but this hardware is so common, basically
every PC I owned for at least 10 years had exactly the same problem - so why can't Microsoft release a compatibility shim
to fix this?

And then, why can't I fix this?

So I wrote this small program, this program does nothing (0% CPU) and ues very little memory (112K on my machine) except
when that annoying windows pops up, then this program leaps into action and makes it the top-most window.

