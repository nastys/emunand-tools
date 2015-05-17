#Emunand Tool Max
Emunand Tool Max is a tool that allows GNU/Linux and OS X users to backup and restore their emuNAND in just a few clicks, without using the terminal. It also allows them to decrypt and mount the emuNAND (thanks to cearp for letting me use 3DSFAT16tool!).

This should have been part of 3DS-CIA Kit, but it was delayed too much due to difficulties in ROMs conversion, so I decided to release its emuNAND Tools part as a standalone app.

This won't be ported to Windows because:

    this tool uses dd and other *NIX related commands to work;
    Windows users can do the same thing with emuNAND Tool (excluding very easy NAND decrypting and mounting :P ).

Features of Emunand Tool Max:

    Backup emuNAND from SD Card
    Restore emuNAND or NAND backup to SD Card
    Decrypt and mount NAND using Decrypt9, 3DSFAT16tool and padxorer
    Supports emuNAND formatted using Gateway and clones
    Material design (unfinished)
    Shows progress with pv (Pipe Viewer) if installed
    Licensed under GNU GPL v3

Planned features:

    Support Mac OS X ASAP done
    Backup all files inside the FAT32 partition or the Nintendo 3DS folder and Launcher.dat only done
    Material design buttons and Backup/Restore icons done (more or less...)
    redNAND creation, backup, restore done
    Encrypt NAND
    Support multi-emuNAND
    Support 2DS backup (I need the NAND size in bytes)
    Autodetect NAND manufacturer (only required to backup the emuNAND)
    emuNAND Backup Manager
    DEB and RPM packages and PPA
    Backup compression
    Save settings
    Optimize CFW Mode

Issues:

    OS X version freezes during backup or restore until done
    OS X version does not check whether the password is correct or not

Changes:

1.3

    CFW Mode; please read below

1.2

    Improved UI
    Backup/restore the Nintendo 3DS folder on the FAT32 partition
    Built on Qt 5.4.1, with native file selection dialogs for GNU/Linux
    Improved file dialogs
    Improved SD card autodetection
    Bug fixes
    Skip caching to SD card during restore on GNU/Linux

1.1

    Ported to OS X
    New layout

Important information about CFW Mode:

This mode allows you to manage the redNAND like a normal emuNAND, so when you restore an emuNAND backup, it gets automatically converted to a redNAND backup. Vice versa, when you backup your redNAND, it gets saved as an emuNAND backup.
If you want to make or restore a redNAND backup, disable this option.
If you want to convert previous redNAND backups to emuNAND backups, use redTools.

How to setup CFW for the first time using ETM and no flashcard:

    Download this file and extract it to the root of your SD card
    Replace the CFW Launcher.dat with the Gateway one
    Open the browser, clear cache and cookies, then go to this page:
        http://tinyurl.com/mp99xlw
    When the console reboots, open System Settings then go to Other Settings > Profile > Nintendo DS Profile
    Select "Format emuNAND" and press A, then START
    When done, go back to the Gateway menu and press D-PAD DOWN+B to power off the console
    Repeat step 1 and replace the Gateway Launcher.dat with the CFW one
    Run Emunand Tool Max and backup the emuNAND
    When done, enable CFW Mode and restore the backup
    When done, unmount the SD card, then put it back into your 3DS and repeat step 4 (it may take many tries to get it working)


It is recommended to backup your entire SD card using Disks at least once before using this for the first time.
Use at your own risk!
