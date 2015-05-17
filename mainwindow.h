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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    bool getRoot();

    void on_detectemuNAND_clicked();

    void on_toshibaNAND_toggled(bool checked);

    void on_samsungNAND_toggled(bool checked);

    void on_detectemuNANDmanufacturer_clicked();

    void on_extractemuNAND_clicked();

    void runShellCommand(QString command);

    void on_restoreemuNAND_clicked();

    void on_copyLauncherdat_2_clicked();

    void on_mountNAND_clicked();

    void mountImage(QString imagefile);

    void on_decryptNAND_clicked();

    void on_unkNAND_toggled(bool checked);

    void on_samsungNAND_2_toggled(bool checked);

    void on_cfwhelp_clicked();

    void on_cfwmode_toggled(bool checked);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
