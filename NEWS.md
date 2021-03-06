# wxMaxima 19.12.4 - Stable:

Since 19.12.3 depending on the system allocated immense amounts of 
RAM and crashed a 19.12.4 had to be released. Together they port
many big improvements to the display code.

 * A Russian translation update by OlesyaGerasimenko
 * The manual is now found on Gentoo, too
 * Big improvements to the maths output
 * A major speedUp by caching already-determined text snippet sizes
 * All icons that aren't provided by the operating system now are
   rendered from SVG
 * Internal communication with maxima is now more MathML-like in
   many places
 * Many additional small bug fixes.


# Transition to C++11 - Part II

Many annoying bugs have to do with memory management. Typical examples are:
 * Out-of-bonds reads and writes: n+1 bytes of an n-bytes-long string are
   read or written. 
 * Use of uninitialized memory: Memory is first read and then set to the 
   value that one would have wanted to read from it.
 * Use-after-free: Memory was first marked as "free" and then used again.
 * Memory Leaks: Memory isn't marked as "free" even if it is no more used.
Against the last two of these bugs C++11 offers automatic pointers that 
stay allocated while they are in used and then are automatically marked 
as free. In the next wxMaxima release about 2500 lines will be changed 
in order to make use of that functionality.

# wxMaxima 19.12.1 - Stable:

If the maxima-discuss list asks for an additional feature that is the Right Time
for a new release:

 * Made the tooltips more visible
 * Added a warning tooltip about missing multiplication signs
 * Sped up drawing of new cells correcting the size calculations
 * If lookalike chars are used in a way that can cause hard-to-find errors now a
   warning tooltip is generated
 * A few additional bug fixes that (besides others) resolve a potential crash
 * An option to turn all multiplication dots on

# wxMaxima 19.12.0 - Stable:

This release provides many under-the-hood changes partially caused by 
switching to C++11 and partially by cppcheck finding bugs and constructs
that can cause bugs later. 

  * wxMaxima now uses C++11 that makes the code more readable and helps
	finding bugs
  * If found wxMaxima now asks cppcheck to find bugs
  * Increased the speed of the program again
  * Opening the config dialogue since 19.11.0 unchecked "offer known answers"
  * For Cmake>=3.10.0 the build system now asks cppcheck to find bugs.
  * Changing the worksheet style was partially broken.
  * A big number of additional bug fixes in various places
  * A config option that forces displaying all multiplication signs.

# Transition to C++11

The next wxMaxima release will make use of C++11 features: I *hope* that C++
has been out long enough in 2019 that every relevant compiler supports it.
C++11 brings in features that allow to make compilation fail on otherwise 
hard-to-detect bugs, some features that might make the code easier to read...
...and I am afraid that plain old c++ might look antiquated one day.

Additionally it will contain many bug fixes hoping that I won't fix bugs that
in retrospect didn't exist... ...on the plus side the program should again 
have gotten faster by giving the compiler additional hints for optimization
and the transition has shrunk wxMaxima by about 100 lines. Much help with
finding bugs came from the cppcheck utility which caused changes in over 
1700 lines of code, mainly in order to prevent possible future bugs.

# wxMaxima 19.11.1 - Stable
  * Improved italian translation by Marco Ciampa
  * Autocompletion no more causes asserts
  * A better logic that decides which cell to send to maxima
  * wxMaxima now supports "-l", "-u" and "-X" command-line args like maxima
  * The help files now are installed in the Right Place
  * Many additional small bug fixes

# wxMaxima 19.11.0 - Stable
  * Chinese translation updates from liulitchi
  * Turkish translation updates from TufanSirin
  * Hungarian translation updates from Blahota István
  * German translation updates from Wolfgang Dautermann and Gunter Königsmann
  * Better detection of the Gnuplot location
  * Better detection of the right place to open autocomplete popups in
  * Better formatting of special unicode operators
  * If autosave on closing fails: Allow the user to ask to exit anyway
  * Enable Autosave even if the option to keep the file saved is disabled
  * A context menu with the "display labels" choices
  * A context menu with "max number of digits" choices
  * A speedup by preventing recursive calls to resize functions
  * Corrected parametric plots in the plot wizard
  * A command-line switch that copies all log messages to stderr.
  * Batch mode no more adds documents to "recent documents"
  * For purists: A "don't offer known answers" config option
  * Now some toolbar items can be disabled
  * Added more toolbar buttons that evaluate cells
  * Many bug fixes

# wxMaxima 19.10.0 - Stable
  * Corrected a Regression: The better display of exponents failed for exp(x^2).
  * Seems like some Windows computers only have a numpad enter key.
  * Chinese translation updates from liulitchi.
  * Russian translation updates from OlesyaGerasimenko.

# wxMaxima 19.09.1 - Stable
  * The names of greek letters are no more converted to greek letters in variable
	names by default
  * Smarter Formatting of asterisk and hyphen
  * An automatic per-commit Windows Build for Appveyor
  * Always recalculate the worksheet size when needed
  * Better vertical alignment of exponents
  * The layout is now more consigstent between platforms
  * Chinese translation updates from liulitchi
  * wxMaxima now remembers the question for each answer, not only its number
  * Now the manual can be localized, too
  * Non-breaking spaces are now replaced by ordinary spaces
    before being sent to maxima
  * As always: Many additional bug fixes

# wxMaxima 19.09.0 - Stable
  * Handle timeouts when writing to maxima and partial writes.
  * Worksheet text disappeared on scrolling on some platforms.
  * Sped up sending code to maxima.
  * Removed quoting of Unicode characters within strings sent to maxima.

# wxMaxima 19.08.0 - Stable
  * Handle timeouts when writing to maxima and partial writes.
  * Worksheet text disappeared on scrolling on some platforms.
  * Sped up sending code to maxima.
  * Removed quoting of Unicode characters within strings sent to maxima.

# wxMaxima 19.07.1 - Stable
  * Bugfixes for nearly every instance something was drawn on the screen.
  * Better handling of multi-cell lisp code

# wxMaxima 19.07.0 - Stable
 * Many improvements and bugfixes in the LaTeX and HTML export function
 * Many updated translations
 * Resolved a freeze on autocompletion
 * A sidebar that shows the contents of variables
 * Simplified the recalculation logic in order to squash the remaining bugs
 * Simplified the sync between input and output cells

# wxMaxima 19.05.7 - Stable
 * In conjunction with a new enough maxima worksheets can now start
   with a :lisp command.
 
# wxMaxima 19.05.6 - Stable
 * Correctly save the autosave interval
 * A better logic for saving on exit if autosave is on and the file 
   cannot be created.
 
# wxMaxima 19.05.5 - Stable
 * An updated hungarian translation
 * The new maxima command cartesian_product_list is now known to
   the autocompletion.
 * Better xim compatibility.

# wxMaxima 19.05.4 - Stable
 * Corrected saving formatting styles in the config dialogue
 * Allow to change the title etc. font again
 * Correctly output uppercase greek letters in TeX
 * Re-enable the old-style ESC commands.
 * Made wxMaxima default to auto-searching for the maxima binary.

# wxMaxima 19.05.3 - Stable
 * Corrected the autowrap line width for high zoom factors
 * Added a few missing "Update the user interface" events

# wxMaxima 19.05.2 - Stable
 * Sometimes the auto-evaluation of the worksheet auto-triggered
 * Support for broken locales that for valid non-ascii chars returned
   isprint()= false
 * Corrected reading font sizes from styles and configuring styles.

# wxMaxima 19.05.1 - Stable
 * Fixes for various combinations of GTK and wxWidgets versions
 * A working Mac version
 * A few Windows bug Fixes
 * Big performance improvements.

# wxMaxima 19.05.0 - Stable
 * Another ArchLinux displaying issue
 * More GTK3 compatibility
 * Resolved a windows hang
 * Better Dark Mode compatibility
 * Performance improvements and bugfixes
 * Many code optimizations.

# wxMaxima 19.04.3 - Stable
 * wxMaxima now informs maxima which front-end is in use.

# wxMaxima 19.04.2 - Stable
 * Corrected the size of error messages
 * A "Copy to mathlab"-feature.
 * Maxima now delays interpreting the data from maxima until it encounters
   a newline or an Timer expires.
 * EMF output no no more causes crashes and strange behaviour.
 * RTF output should now work again
 * entermatrix() now works again.
 * Better help file detection on MS Windows.

# wxMaxima 19.04.1 - Stable
 * Corrected the size of error messages
 
# wxMaxima 19.04.0 - Stable

 * The cursor width now is taken from the current screen
 * Autocompletion sometimes cleared the result after inputting it
 * Added more commands to autocompletion
 * Spanish and italian translation updates
 * We no more need to re-implement maxima's load() routine.
   Instead we call the original one now.
 * Animations now on load remember which slide they stopped at.
 * A more error-proof language selection.
 * If the system looks like it uses UTF8 by default the locale name
   that is passed to maxima now ends in ".UTF8" which should resolve
   the "setting locale failed" errors from maxima.
 * Corrected the initial size of error messages.
 * Many additional bug fixes
 * Code cleanups


# wxMaxima 19.03.1 - Stable

 * HighDPI fixes for ArchLinux, Windows and SuSE
 * Resolved crashes on RedHat and MS Windows
 * SBCL's compilation messages now appear in the status bar, not
   in the worksheet.
 * Removed an unneeded "maxima has finished calculating" on startup.
 * Many small bug fixes.
 * wxWidgets >= 3.1.0: Corrected the toolbar icon size.

# wxMaxima 19.03.0 - Stable

 * The ESC sequences for inputting symbols now use autocompletion.
 * Use wxAutoBufferedPaintDC for drawing the worksheet instead of
   implementing our own version of it.
 * The usual bug fixes, this time including one bug fix that makes
   wxMaxima work again on Gentoo and help path lookup fixes for
   different operating systems.

# wxMaxima 19.02.2 - Stable

 * Corrected the GTK3 fix
 * Cleaned up the cleanup process on closing wxMaxima

# wxMaxima 19.02.1 - Stable

* Support for the broken ArchLinux compiler
* Resolved the Crash on Closing for Mac Os
* A "File open" bug that was depending on the language setting
* Corrected scrolling on wxWidgets 3.1.3 for GTK3

# wxMaxima 19.02.0 - Stable

* Resolved a cell size recalculation bug
* Better wxWidgets 3.1.2 compatibility
* Improved performance

# wxMaxima 19.01.3 - Stable

* Repaired compilation on MacOs for wxWidgets 3.1.1.
* Better path detection on MS Windows
* The "-m" switch needed for the appimage

# wxMaxima 19.01.2 - Stable

* Corrected the calculation of image heights
* Corrected the line break algorithm for printing.
