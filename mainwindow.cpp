/*
 * Copyright © 2015 nastys
 *
 * This file is part of Emunand Tool Max.
 * Emunand Tool Max is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emunand Tool Max is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emunand Tool Max.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QDesktopServices>

int nandtype=-1; // 0=old-toshiba; 1=old-samsung; 2=new-unk; 3=new-samsung
QString emunanddevice, emunandname="", mounted="", romFile, rootpwd="", dd="dd", ddroot="sudo dd", endb[]={"GATEWAYNAND", "MTCARDNAND", "3DSCARDNAND"};
QFile pv("/usr/bin/pv");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_detectemuNAND_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::getRoot()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "groups | grep -i \"disk\"");
    process.waitForFinished(-1);
    process.waitForReadyRead(-1);
    if(process.readAllStandardOutput().simplified()==""&&rootpwd=="")
    {
        for(bool correct=0; correct==0;)
        {
            bool ok;
            rootpwd=QInputDialog::getText(this, "Emunand Tool Max", "This tool needs root access.\nPlease enter your password below.\n\nAlternatively, you can add yourself to the disk group.", QLineEdit::Password, "", &ok);
            if(ok==0)
                return 1;
            ddroot="echo -e \""+rootpwd+"\"| sudo -S dd";
            dd=ddroot;
            runShellCommand("echo -e \""+rootpwd+"\"| sudo -S touch /tmp/.3ds_sudo-success");
            QFile sudosuccess("/tmp/.3ds_sudo-success");
            if(sudosuccess.exists())
                correct=1;
            runShellCommand("echo -e \""+rootpwd+"\"| sudo -S rm /tmp/.3ds_sudo-success");
        }
    }

    return 0;
}

void MainWindow::on_detectemuNAND_clicked()
{
    emunanddevice="";
    ui->emuNANDdevice->clear();
    ui->extractemuNAND->setEnabled(false);
    ui->restoreemuNAND->setEnabled(false);

    QMessageBox::information(this, "emuNAND detection", "Please mount the SD card containing the emuNAND and press OK.\nPlease note that only one SD card at a time is supported.");
    for(int i=0; i<4 && emunanddevice==""; i++) {
        QString enn;

        bool ok=1;

        if(i==3)
        {
            QString mn=QInputDialog::getText(this, "emuNAND detection", "Please enter the label of the FAT32 partition of the SD card: ", QLineEdit::Normal, "GATEWAYNAND", &ok);
            enn=mn;
        }
        else
            enn=endb[i];

        if(ok)
        {
            QProcess process;
            process.start("bash", QStringList() << "-c" << "mount | grep -i \""+enn+"\"");
            if(process.waitForStarted(-1))
            {
                while(process.waitForReadyRead(-1))
                {
                    emunanddevice=process.readAllStandardOutput().simplified();
                    mounted=emunanddevice.section(" ", 2, 2);
                    emunanddevice=emunanddevice.section(" ", 0, 0);
                    emunanddevice=emunanddevice.remove(emunanddevice.length()-1, 1);
                    if((emunanddevice.endsWith("p")|emunanddevice.endsWith("s")))
                        emunanddevice=emunanddevice.remove(emunanddevice.length()-1, 2);
                }
            }
        }
    }

    if(emunanddevice=="")
    {
        QMessageBox::critical(this, "emuNAND not detected", "Couldn't detect the emuNAND.\nIs the SD card mounted?");
        ui->extractemuNAND->setEnabled(false);
        ui->restoreemuNAND->setEnabled(false);
    }
    else
    {
        if((ui->toshibaNAND->isChecked()==true) | (ui->samsungNAND->isChecked()==true))
            ui->extractemuNAND->setEnabled(true);
        ui->restoreemuNAND->setEnabled(true);
    }
    ui->emuNANDdevice->setText(emunanddevice);
}

void MainWindow::on_toshibaNAND_toggled(bool checked)
{
    if(checked==true)
    {
        nandtype=0;
        if(emunanddevice!="")
            ui->extractemuNAND->setEnabled(true);
    }
}

void MainWindow::on_samsungNAND_toggled(bool checked)
{
    if(checked==true)
    {
        nandtype=1;
        if(emunanddevice!="")
            ui->extractemuNAND->setEnabled(true);
    }
}

void MainWindow::on_detectemuNANDmanufacturer_clicked()
{
    if(QMessageBox::information(this, "Detect NAND manufacturer", "In order to backup the emuNAND, you need to know the manufacturer of your NAND.\nI can detect it for you if you have a NAND backup.", "I have a NAND backup", "I don't have a NAND backup"))
        QMessageBox::critical(this, "Detect NAND manufacturer", "Sorry, I cannot detect the manufacturer without a NAND backup.");
    else
    {
        QString backuppath = QFileDialog::getOpenFileName(this, "Open NAND backup", "", "NAND backup (NAND.BIN);;Binary files (*.bin);;All files (*)");
        if(backuppath=="")
            return;
        QFile backupfile(backuppath);
        backupfile.open(QIODevice::ReadOnly);
        if(backupfile.size()==988807168)
        {
            nandtype=0;
            ui->toshibaNAND->setChecked(true);
            QMessageBox::information(this, "Detect NAND manufacturer", "Your NAND takes up "+QString::number(backupfile.size())+" bytes, so it's a Toshiba NAND.");
            if(emunanddevice!="")
                ui->extractemuNAND->setEnabled(true);
        }
        else if(backupfile.size()==1000341504)
        {
            nandtype=1;
            ui->samsungNAND->setChecked(true);
            QMessageBox::information(this, "Detect NAND manufacturer", "Your NAND takes up "+QString::number(backupfile.size())+" bytes, so it's a Samsung NAND.");
            if(emunanddevice!="")
                ui->extractemuNAND->setEnabled(true);
        }
        else if(backupfile.size()==1979711488)
        {
            nandtype=2;
            ui->unkNAND->setChecked(true);
            QMessageBox::information(this, "Detect NAND manufacturer", "Your NAND takes up "+QString::number(backupfile.size())+" bytes, so it's a New 3DS NAND.");
            if(emunanddevice!="")
                ui->extractemuNAND->setEnabled(true);
        }
        else if(backupfile.size()==1300234240)
        {
            nandtype=3;
            ui->samsungNAND_2->setChecked(true);
            QMessageBox::information(this, "Detect NAND manufacturer", "Your NAND takes up "+QString::number(backupfile.size())+" bytes, so it's a New 3DS Samsung NAND.");
            if(emunanddevice!="")
                ui->extractemuNAND->setEnabled(true);
        }
        else
            QMessageBox::warning(this, "Detect NAND manufacturer", "WARNING\nYour NAND size takes up "+QString::number(backupfile.size())+" bytes.\nI don't know about this NAND. Is your backup valid?\nBackup your NAND again and if this problem persists, please report your NAND size.");

        backupfile.close();
    }
}

void MainWindow::on_extractemuNAND_clicked()
{
    if(getRoot())
        return;

    QString saveas=QFileDialog::getSaveFileName(this, "Select where to SAVE the emuNAND backup", "emuNAND.BIN","All files (*)");
    if(saveas!="")
    {
        if(!saveas.endsWith(".bin", Qt::CaseInsensitive))
            saveas+=".bin";

        ui->detectemuNAND->setEnabled(false);
        ui->toshibaNAND->setEnabled(false);
        ui->samsungNAND->setEnabled(false);
        ui->unkNAND->setEnabled(false);
        ui->samsungNAND_2->setEnabled(false);
        ui->detectemuNANDmanufacturer->setEnabled(false);
        ui->extractemuNAND->setEnabled(false);
        ui->n3dsfolder->setEnabled(false);

        QString count2, skip1, ext;

        if(ui->cfwmode->isChecked())
            ext=".tmp";
        else
            ext="";

        if(nandtype==0)
        {
            count2="1931263";
            skip1="1931264";
        }
        else if(nandtype==1)
        {
            count2="1953791";
            skip1="1953792";
        }
        else if(nandtype==2)
        {
            count2="3866623";
            skip1="3866624";
        }
        else if(nandtype==3)
        {
            count2="2539519";
            skip1="2539520";
        }
        else
            return;

        runShellCommand("rm \""+saveas+"\" \""+saveas+".mbr\" \""+saveas+".3DS.tar\" \""+saveas+ext+"\"");

        if(pv.exists())
        {
            if(ui->cfwmode->isChecked()==0)
                runShellCommand(dd+" if="+emunanddevice+" | pv | dd of=\""+saveas+"\".mbr count=1");
            runShellCommand(dd+" if="+emunanddevice+" | pv | dd of=\""+saveas+ext+"\" count=1 skip="+skip1);
            runShellCommand(dd+" if="+emunanddevice+" | pv | dd of=\""+saveas+ext+"\" count="+count2+" skip=1 seek=1");
            if(ui->cfwmode->isChecked())
                runShellCommand("dd bs=512 skip=1 if=\""+saveas+ext+"\" | pv | dd of=\""+saveas+"\"");
        }
        else
        {
            if(ui->cfwmode->isChecked()==0)
                runShellCommand(dd+" if="+emunanddevice+" | dd of=\""+saveas+"\".mbr count=1");
            runShellCommand(dd+" if="+emunanddevice+" | dd of=\""+saveas+ext+"\" count=1 skip="+skip1);
            runShellCommand(dd+" if="+emunanddevice+" | dd of=\""+saveas+ext+"\" count="+count2+" skip=1 seek=1");
            if(ui->cfwmode->isChecked())
                runShellCommand("dd bs=512 skip=1 if=\""+saveas+ext+"\" of=\""+saveas+"\"");
        }

        if(ui->cfwmode->isChecked())
            runShellCommand("rm \""+saveas+ext+"\"");

        if(ui->n3dsfolder->isChecked())
        {
            runShellCommand("tar -cvf \""+saveas+".3DS.tar\" -C \""+mounted+"/\" \"Nintendo 3DS/\"");
        }

        ui->detectemuNAND->setEnabled(true);
        ui->toshibaNAND->setEnabled(true);
        ui->samsungNAND->setEnabled(true);
        ui->unkNAND->setEnabled(true);
        ui->samsungNAND_2->setEnabled(true);
        ui->detectemuNANDmanufacturer->setEnabled(true);
        ui->extractemuNAND->setEnabled(true);
        ui->n3dsfolder->setEnabled(true);

        QMessageBox::information(this, "Backup emuNAND", "Done!");
    }
}

void MainWindow::runShellCommand(QString command)
{
    QFile xterm("/usr/bin/xterm");
    QProcess shell;
    if(xterm.exists())
        shell.start("xterm", QStringList() << "-e" << command);
    else
        shell.start("bash", QStringList() << "-c" << command);
    shell.waitForFinished(-1);
}

void MainWindow::on_restoreemuNAND_clicked()
{
    if(getRoot())
        return;

    QString backuppath=QFileDialog::getOpenFileName(this, "Select the (emu)NAND backup to RESTORE", "","All files (*)");

    if(backuppath!="")
    {
        QFile backupfile(backuppath);
        QString seek=QString::number(backupfile.size()/512);

        ui->detectemuNAND->setEnabled(false);
        ui->toshibaNAND->setEnabled(false);
        ui->samsungNAND->setEnabled(false);
        ui->unkNAND->setEnabled(false);
        ui->samsungNAND_2->setEnabled(false);
        ui->detectemuNANDmanufacturer->setEnabled(false);
        ui->extractemuNAND->setEnabled(false);
        ui->n3dsfolder->setEnabled(false);

        QFile mbr(backuppath+".mbr");
        QString mbrpath="";
        if(mbr.exists())
        {
            if(QMessageBox::question(this, "Restore emuNAND to SD card", "Do you want to restore the MBR?\nIf you are unsure, just click No.", "No", "Yes"))
                mbrpath = backuppath+".mbr";
        }

        QFile tarball(backuppath+".3DS.tar");
        if(ui->n3dsfolder->isChecked() && tarball.exists())
        {
            runShellCommand("rm -r \""+mounted+"/Nintendo 3DS/\"");
            runShellCommand("tar -xvf \""+backuppath+".3DS.tar\" -C \""+mounted+"/\" \"Nintendo 3DS/\"");
        }

        if(rootpwd=="")
            runShellCommand("rm \"/tmp/3dsciakit-nand.tmp\" \"/tmp/3dsciakit-final.tmp\" \"/tmp/3dsciakit-mbr.tmp\"");
        else
            runShellCommand("echo -e \""+rootpwd+"\"| sudo -S rm \"/tmp/3dsciakit-nand.tmp\" \"/tmp/3dsciakit-final.tmp\" \"/tmp/3dsciakit-mbr.tmp\"");



        if(ui->cfwmode->isChecked())
        {
            runShellCommand("fallocate -l 512 \"/tmp/3dsciakit-nand.tmp\"");
            if(pv.exists())
                runShellCommand("dd if=\""+backuppath+"\" | pv | dd of=\"/tmp/3dsciakit-nand.tmp\" seek=1");
            else
                runShellCommand("dd if=\""+backuppath+"\" of=\"/tmp/3dsciakit-nand.tmp\" seek=1");
            backuppath="\"/tmp/3dsciakit-nand.tmp\"";
        }

        if(pv.exists())
        {
            if(mbrpath=="")
            {
                runShellCommand(dd+" if="+emunanddevice+" | pv | dd of=\"/tmp/3dsciakit-mbr.tmp\" count=1");
                mbrpath="/tmp/3dsciakit-mbr.tmp";
            }
            runShellCommand(dd+" if=\""+mbrpath+"\" | pv | dd of=\"/tmp/3dsciakit-final.tmp\" count=1");
            runShellCommand(dd+" if=\""+backuppath+"\" | pv | dd of=\"/tmp/3dsciakit-final.tmp\" skip=1 seek=1");
            runShellCommand(dd+" if=\""+backuppath+"\" | pv | dd of=\"/tmp/3dsciakit-final.tmp\" count=1 seek="+seek);
        }
        else
        {
            if(mbrpath=="")
            {
                runShellCommand(dd+" if="+emunanddevice+" of=\"/tmp/3dsciakit-mbr.tmp\" count=1");
                mbrpath="/tmp/3dsciakit-mbr.tmp";
            }
            runShellCommand(dd+" if=\""+mbrpath+"\" of=\"/tmp/3dsciakit-final.tmp\" count=1");
            runShellCommand(dd+" if=\""+backuppath+"\" of=\"/tmp/3dsciakit-final.tmp\" skip=1 seek=1");
            runShellCommand(dd+" if=\""+backuppath+"\" of=\"/tmp/3dsciakit-final.tmp\" count=1 seek="+seek);
        }

#ifdef __APPLE__
            runShellCommand("diskutil unmountDisk "+emunanddevice);
            runShellCommand(dd+" if=\"/tmp/3dsciakit-final.tmp\" of=\""+emunanddevice+"\" bs=8000000");
#else
            runShellCommand(dd+" if=\"/tmp/3dsciakit-final.tmp\" of=\""+emunanddevice+"\" bs=8000000 oflag=direct");
#endif

            if(rootpwd=="")
                runShellCommand("rm \"/tmp/3dsciakit-nand.tmp\" \"/tmp/3dsciakit-final.tmp\" \"/tmp/3dsciakit-mbr.tmp\"");
            else
                runShellCommand("echo -e \""+rootpwd+"\"| sudo -S rm \"/tmp/3dsciakit-nand.tmp\" \"/tmp/3dsciakit-final.tmp\" \"/tmp/3dsciakit-mbr.tmp\"");

        ui->detectemuNAND->setEnabled(true);
        ui->toshibaNAND->setEnabled(true);
        ui->samsungNAND->setEnabled(true);
        ui->unkNAND->setEnabled(true);
        ui->samsungNAND_2->setEnabled(true);
        ui->detectemuNANDmanufacturer->setEnabled(true);
        if(nandtype>-1)
            ui->extractemuNAND->setEnabled(true);
        ui->n3dsfolder->setEnabled(true);

        QMessageBox::information(this, "Restore emuNAND to SD card", "Restore complete.\nNow you can unmount the SD card.");
    }
}

void MainWindow::on_copyLauncherdat_2_clicked()
{
    QString saveas=QFileDialog::getSaveFileName(this, "Save Decryptor9 Launcher.dat", "Launcher.dat", "Launcher (Launcher.dat);;Data files (*.dat);;All files (*)");
    if(saveas!="")
    {
        QProcess process;
        process.start("cp \"tools/Decrypt9/Launcher.dat\" \""+saveas+"\"");
        process.waitForFinished(-1);
    }
}

void MainWindow::on_mountNAND_clicked()
{
    mountImage("");
}

void MainWindow::on_decryptNAND_clicked()
{
    QString nand=QFileDialog::getOpenFileName(this, "Open NAND to decrypt", "", "All files (*)");
    if(nand!="")
    {
        QString nandxorpad=QFileDialog::getOpenFileName(this, "Open NAND xorpad", "", "NAND xorpad (nand.fat16.xorpad);;Xorpad (*.xorpad);;All files (*)");
        if(nandxorpad!="")
        {
            QString nandout=QFileDialog::getSaveFileName(this, "Save decrypted NAND", "", "Image file (*.img)");
            if(nandout!="")
            {
                if(!nandout.endsWith(".img", Qt::CaseInsensitive))
                    nandout+=".img";
                runShellCommand("python \"tools/3DSFAT16tool.py\" -d \""+nand+"\" \""+nandout+"~\"");
                runShellCommand("tools/padxorer \""+nandout+"~\" \""+nandxorpad+"\"");
                runShellCommand("rm \""+nandout+"~\"");
                runShellCommand("mv \""+nandout+"~.out\" \""+nandout+"\"");
                if(!QMessageBox::question(this, "Decrypt NAND", "Decryption complete. Would you like to mount the decrypted NAND now?\n\nNOTE: you can mount it by simply opening it using Disk Image Mounter.", "Yes", "No"))
                    mountImage(nandout);
            }
        }
    }
}

void MainWindow::mountImage(QString imagefile)
{
#ifdef __APPLE__
    if(imagefile=="")
    {
        imagefile=QFileDialog::getOpenFileName(this, "Mount decrypted NAND", "", "Image files (*.img);;All files (*)");
        if(imagefile=="")
            return;
    }
    QProcess process;
    process.start("hdiutil attach \""+imagefile+"\"");
    process.waitForFinished(-1);
#else
    QFile mounter("/usr/bin/gnome-disk-image-mounter");
    if(mounter.exists())
    {
        QProcess process;
        process.start("gnome-disk-image-mounter \""+imagefile+"\"");
        process.waitForFinished(-1);
    }
    else
    {
        if(!QMessageBox::question(this, "Mount decrypted NAND", "This action requires gnome-disk-utility.\nDo you want to install it?", "Yes", "No"))
        {
            QDesktopServices::openUrl(QUrl("apt://gnome-disk-utility"));
        }
    }
#endif
}

void MainWindow::on_unkNAND_toggled(bool checked)
{
    if(checked==true)
    {
        nandtype=2;
        if(emunanddevice!="")
            ui->extractemuNAND->setEnabled(true);
    }
}

void MainWindow::on_samsungNAND_2_toggled(bool checked)
{
    if(checked==true)
    {
        nandtype=3;
        if(emunanddevice!="")
            ui->extractemuNAND->setEnabled(true);
    }
}

void MainWindow::on_cfwhelp_clicked()
{
    QMessageBox::information(this, "CFW Mode", "With this option enabled, Emunand Tool Max automatically converts an emuNAND backup to a redNAND backup during restore and a redNAND backup to an emuNAND backup during backup.\nIn other words, the input and output files are emuNAND backups, while the NAND stored in your SD card is a redNAND one.\nDo NOT restore a redNAND backup with this option enabled!");
}

void MainWindow::on_cfwmode_toggled(bool checked)
{
    if(checked)
        ui->spaceinfo->setText("<html><head/><body><p>Both emuNAND Backup and Restore require 2 GB of free disk space.</p></body></html>");
    else
        ui->spaceinfo->setText("<html><head/><body><p>Both emuNAND Backup and Restore require 1 GB of free disk space.</p></body></html>");
}
