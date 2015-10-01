# INTRODUCTION #

Hey, what's this ? a project written in Borland C++ with OWL ? are you kidding ?

No, I'm not kidding.

I was working for a small French software editor, who was developing accounting solutions for French public hospitals and resting homes. It mainly addressed human resources (payroll), accounting, invoicing of resting fees and a few more accounting services.

The ground basis of this solution was developed by Claude Chartier, in 1988 ; it ran initially on Matra systems (on a model I can't remember the name of...) then was re-engineered on Unix platforms (more precisely, on SCO/Xenix then, later, on SCO/Unix) in 1989, I guess. Of course, at those times, the solution worked in character mode using *termcap* and *curses* ; there were no graphic ornaments, and semi-graphic characters were a wish-to-have.

The underlying database was some kind of indexed-sequential table manager, which was developed in one week by Claude Chartier (as the legend says...), the co-founder of the Company, **Chartier Informatique**. The database API was not very elaborated when you wanted to run queries, but it was compact, simple and efficient. 

The whole solution, named **Cheops**, met the requirements of our customers : you can easily imagine that small resting homes, or middle-sized hospitals in the mid-90s could not afford to buy mainframes ! instead, they had a PC server running SCO/Unix, and passive terminals for their users to accomplish their day-to-day reporting work. That was a clearly reasonable financial compromise for those structures.

# THE GENESIS #

I joined the team in 1991. We were nine in total.

The first thing that stroke me is the intelligence that had been put onto the hotline ; not only were the hotliners developers, trainers and site installers, they also had the right tools to achieve their work. In 1991 (and quite since the beginning of the story in 1989), they were able to connect to their customer's server using a modem, which saved a huge time to everybody, and avoided so much travels to the customer's site to see what happens...

This may sound curious nowadays, when any jerk can use the power of the net to join you, again and again, and potentially take the control of your PC, tablet, smartphone, car and coffee machine. But that was not the case in the early 90s.

During the next two years (1991-1993), we saw that the things were evolving : Windows 3.1 was arising, and, together with it, Ethernet connections started to replace simple Rs232 connections.

Our customers started purchasing Windows PCs ; it did not take too much time before they become aware that it would be a good thing if they could use their favorite Windows software and access their **Cheops** server from the same machine. And get rid of passive terminals connected over an rs232 line, whose quality was often impacted by the distance between the server and the terminal.

So came the Ethernet boards ; then we started hearing about Windows 95... and about new graphical solutions developed by our competitors. We clearly began to suffer from the character-based interface we were providing.    

# THE PROJECT #

So, there was a real need to provide a graphical user interface to our customers ; but how to do that with 9 people in the company, who were all busy with their day-to-day tasks ?

(9 people = 1 CEO, 1 Sales Director, 1 assistant, 2.5 developers in average and 3.5 hotliners/installers/trainers).

I was asked to find such a solution. Well, I was aware that we could not rewrite (re-engineer) the whole software solution : filling the gap between a character-based software written in C relying on an indexed-sequential database and state-of-the-art development environments and middleware would have killed the company. 

Another strong constraint was that we couldn't ask our customers to purchase new hardware ; remember, we were working for public institutional customers, and the budgeting cycle is very long in France for such public institutions...

The **Cheops** applications were architectured around a basic, simple menu ; not a pop-up menu that could be displayed using the Unix *curses* API, but a flat-style menu, that occupied the whole page (à la IBM...).

This menu was used to launch two kinds of programs : forms and report generators. And nothing else.

So I suggested the following solution : "Ok, let's say our customers keep their existing servers and passive terminals ; let's say that we let them purchase new PCs in the future and that we want those PCs to access their **Cheops** server ; why not develop an intermediate process that could behave differently depending on whether the user device is a passive terminal or a PC running Windows ?"

Of course, in the case where the user is running Windows, this would require a client application to interpret brand new graphical directives. This is how this application, **Terminal**, was born.

It runs basically as a terminal emulator (like **putty**) but, if the process on the other side sends the appropriate escape sequence, then it will act as some kind of "Internet Explorer avant l'heure" and be able to get form definitions from the server, send user interactions back to the server and so on.

On the Unix side, a server process was started for each connecting user. It could handle connections through passive terminals or Windows platforms using the **Terminal** emulator.

For passive terminal connections, this server process just served the basic *curses* API to display the data and get user input. One positive side-effect of this implementation is that users could now have several forms opened at the same time, and switch back and forth each of them.

For Windows platforms running the **Terminal** application, the *curses* API was replaced with a custom one, which used Windows resource files to describe dialog boxes. In fact, dialog boxes were designed using Borland C++ then converted to binary format on the server side. Then downloaded on the client side whenever the user wanted to access a form, the application being responsible for rendering the form. It was some kind of very specialized Internet Explorer...

# CREDITS #

Of course I was not alone on this project ; everything started with me and a few talented students in their end of study project ; among them are the first ones, **Sébastien George** and **Christophe Després**, who developed the bases of the terminal emulator (well, they really developed a terminal emulator in two months...).

And another very talented developer, **Jean-Luc Lebresne**, who became a colleague and worked with me until the end of the project.

 
# TECHNICAL LESSONS LEARNED #

Well, we had to strive through the Windows API, itself encapsulated by Borland OWL ; we had to cope with terminal emulation over rs232 and Ethernet links, window scrolling, text selection using the mouse (avoiding flickering), manipulating graphical regions, asynchronous communications, custom graphical gadgets and popup menus, and so on.

There was no multi-threading at those times since we started with Windows 3.1, then 3.11, 95 and 98... 

We were rebooting our PCs at least ten times a day, due to instabilities. But I always kept on thinking about former colleagues from the first company I worked for, who were working in the early 70s in assembly language on punched cards, and surely welcomed the arrival of teletypes with a great pleasure !

# WHY PUBLISHING SUCH A DEAD PROJECT ?

*in memoriam*... 

More seriously :

- This has been a proof on how to find elegant solutions with so few resources and so few budget
- This has been a travel through the quirks of the Windows API (and notably GDI), which still partly undermines current versions of Windows
- Due to the economical and time constraints, this has been an architectural challenge. Maybe no school can teach you that.
- Time pressure was here, but it did not globally affect our ability to invent new solutions. Sometimes we were stagnating, and sometimes we were creative...
