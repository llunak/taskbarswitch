/********************************************************************
Copyright (C) 2009 Lubos Lunak <l.lunak@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "taskbarswitch.h"

#include <qaction.h>
#include <qdebug.h>
#include <kactioncollection.h>
#include <kglobalaccel.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kwindowsystem.h>
#include <limits.h>
#include <netwm.h>
#include <qx11info_x11.h>

K_PLUGIN_FACTORY(TaskbarSwitchFactory,
                 registerPlugin<TaskbarSwitchModule>();
    )
K_EXPORT_PLUGIN(TaskbarSwitchFactory("taskbarswitch"))

TaskbarSwitchModule::TaskbarSwitchModule( QObject* parent, const QList<QVariant>& )
    : KDEDModule( parent )
    {
    KLocalizedString::setApplicationDomain( "taskbarswitch" );
    setModuleName( "taskbarswitch" );
    }

#ifdef TASKBARSWITCH_STANDALONE
#include <kapplication.h>
#include <kcmdlineargs.h>
int main( int argc, char* argv[] )
    {
    KCmdLineArgs::init( argc, argv, "a", "b", ki18n( "c" ), "d" );
    KApplication app;
    TaskbarSwitch sw;
    return app.exec();
    }
#endif


TaskbarSwitch::TaskbarSwitch()
    {
    KActionCollection *ac = new KActionCollection( this );
    ac->setComponentName( "taskbarswitch" );
    QAction* a = NULL;
    a = ac->addAction( "Taskbar Item Left" );
    a->setText( i18n( "Switch One Taskbar Item to the Left" ));
    KGlobalAccel::setGlobalShortcut( a, QKeySequence( Qt::META+Qt::Key_Left ));
    connect( a, SIGNAL( triggered()), SLOT( activateTaskbarItemLeft()));
    a = ac->addAction( "Taskbar Item Right" );
    a->setText( i18n( "Switch One Taskbar Item to the Right" ));
    KGlobalAccel::setGlobalShortcut( a, QKeySequence( Qt::META+Qt::Key_Right ));
    connect( a, SIGNAL( triggered()), SLOT( activateTaskbarItemRight()));
    }

void TaskbarSwitch::activateTaskbarItemLeft()
    {
    buildData();
    WId active = KWindowSystem::activeWindow();
    if( !rects.contains( active ))
        {
        activateWindowDesktopLeft();
        return;
        }
    if( activateTaskbarItemLeft( rects[ active ].x()))
        return;
    activateTaskbarItemLeft( INT_MAX );
    }

bool TaskbarSwitch::activateTaskbarItemLeft( int pos )
    {
    int bestpos = INT_MIN;
    WId bestw;
    for( QHash< WId, QRect >::ConstIterator it = rects.begin();
         it != rects.end();
         ++it )
        {
        WId w = it.key();
        if( rects[ w ].x() > bestpos && rects[ w ].x() < pos )
            {
            bestpos = rects[ w ].x();
            bestw = w;
            }
        }
    if( bestpos != INT_MIN )
        {
        KWindowSystem::forceActiveWindow( bestw );
        return true;
        }
    return false;
    }

void TaskbarSwitch::activateWindowDesktopLeft()
    {
    int cur = KWindowSystem::currentDesktop();
    for( int desktop = cur - 1;
         desktop > 0;
         --desktop )
        {
        if( activateWindowDesktopLeft( desktop ))
            return;
        }
    for( int desktop = KWindowSystem::numberOfDesktops();
         desktop > cur;
         --desktop )
        {
        if( activateWindowDesktopLeft( desktop ))
            return;
        }
    }

bool TaskbarSwitch::activateWindowDesktopLeft( int desktop )
    {
    int bestpos = INT_MIN;
    WId bestw;
    for( QHash< WId, int >::ConstIterator it = desktops.begin();
         it != desktops.end();
         ++it )
        {
        WId w = it.key();
        if( *it == desktop && bestpos < rects[ w ].x())
            {
            bestpos = rects[ w ].x();
            bestw = w;
            }
        }
    if( bestpos != INT_MIN )
        {
        KWindowSystem::forceActiveWindow( bestw );
        return true;
        }
    return false;
    }

void TaskbarSwitch::activateTaskbarItemRight()
    {
    buildData();
    WId active = KWindowSystem::activeWindow();
    if( !rects.contains( active ))
        {
        activateWindowDesktopRight();
        return;
        }
    if( activateTaskbarItemRight( rects[ active ].x()))
        return;
    activateTaskbarItemRight( INT_MIN );
    }

bool TaskbarSwitch::activateTaskbarItemRight( int pos )
    {
    int bestpos = INT_MAX;
    WId bestw;
    for( QHash< WId, QRect >::ConstIterator it = rects.begin();
         it != rects.end();
         ++it )
        {
        WId w = it.key();
        if( rects[ w ].x() < bestpos && rects[ w ].x() > pos )
            {
            bestpos = rects[ w ].x();
            bestw = w;
            }
        }
    if( bestpos != INT_MAX )
        {
        KWindowSystem::forceActiveWindow( bestw );
        return true;
        }
    return false;
    }

void TaskbarSwitch::activateWindowDesktopRight()
    {
    int cur = KWindowSystem::currentDesktop();
    int max = KWindowSystem::numberOfDesktops();
    for( int desktop = cur + 1;
         desktop < max;
         ++desktop )
        {
        if( activateWindowDesktopRight( desktop ))
            return;
        }
    for( int desktop = 0;
         desktop < cur;
         ++desktop )
        {
        if( activateWindowDesktopRight( desktop ))
            return;
        }
    }

bool TaskbarSwitch::activateWindowDesktopRight( int desktop )
    {
    int bestpos = INT_MAX;
    WId bestw;
    for( QHash< WId, int >::ConstIterator it = desktops.begin();
         it != desktops.end();
         ++it )
        {
        WId w = it.key();
        if( *it == desktop && bestpos > rects[ w ].x())
            {
            bestpos = rects[ w ].x();
            bestw = w;
            }
        }
    if( bestpos != INT_MAX )
        {
        KWindowSystem::forceActiveWindow( bestw );
        return true;
        }
    return false;
    }

void TaskbarSwitch::buildData()
    {
// it appears to be more efficient to actually fetch the data whenever needed, rather than
// update it all the time
    rects.clear();
    desktops.clear();
    foreach( WId w, KWindowSystem::windows())
        {
        NETWinInfo inf( QX11Info::connection(), w, QX11Info::appRootWindow(), NET::WMIconGeometry | NET::WMDesktop, 0 );
        const NETRect r = inf.iconGeometry();
        if( r.size.width != 0 )
            rects[ w ] = QRect( r.pos.x, r.pos.y, r.size.width, r.size.height );
        desktops[ w ] = inf.desktop();
        }
    }

#include "taskbarswitch.moc"
