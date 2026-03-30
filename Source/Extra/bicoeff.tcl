package require Tk

# global variables
namespace eval bicoeff:: {
    # array of 'my' instance IDs in a given tkcanvas
    variable mys_in_tkcanvas
    # color
    variable markercolor "#bbbbcc"
}

#------------------------------------------------------------------------------#
proc bicoeff::send_params {my x y} {
    variable ${my}::bind_name
    variable ${my}::filtercenter
    variable ${my}::filterwidth
    variable ${my}::filtergain
    pdsend "$bind_name _params $filtercenter $filterwidth $filtergain $x $y"
}

# move  x/y axis
proc bicoeff::start_move_xy {my x y} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable ${my}::previousx $x
    variable ${my}::previousy $y
    variable ${my}::framey1
    variable ${my}::framey2
    variable ${my}::filtercenter
    variable markercolor
    $tkcanvas bind $tag <Motion> "bicoeff::move_xy $my %x %y"
}

proc bicoeff::move_xy {my x y} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable ${my}::previousx
    variable ${my}::framex1
    variable ${my}::framey1
    variable ${my}::framex2
    variable ${my}::framey2
    variable ${my}::filterx1
    variable ${my}::filterx2
    variable ${my}::filterwidth
    variable ${my}::filtercenter
    variable ${my}::previousy
    variable ${my}::gainy
    variable ${my}::filtergain
# move x axis
    set dx [expr $x - $previousx]
    set x1 [expr $filterx1 + $dx]
    set x2 [expr $filterx2 + $dx]
    if {$x1 < $framex1} {
        set filterx1 $framex1
        set filterx2 [expr $framex1 + $filterwidth]
    } elseif {$x2 > $framex2} {
        set filterx1 [expr $framex2 - $filterwidth]
        set filterx2 $framex2
    } else {
        set filterx1 $x1
        set filterx2 $x2
    }
    set filterwidth [expr $filterx2 - $filterx1]
    set filtercenter [expr $filterx1 + ($filterwidth/2)]
    $tkcanvas coords bandleft$tag $filterx1 $framey1  $filterx1 $framey2
    $tkcanvas coords bandright$tag $filterx2 $framey1  $filterx2 $framey2
    set previousx $x
# move y axis
    set gain [expr $filtergain + $y - $previousy]
    set framemax [expr $framey2 - $framey1]
    if {[expr $gain < 0]} {
        set filtergain 0
    } elseif {[expr $gain > $framemax]} {
        set filtergain $framemax
    } else {
        set filtergain $gain
    }
    set gainy [expr $framey1 + $filtergain]
    $tkcanvas coords $framex1 $gainy $framex2 $gainy
    set previousy $y
# send values
    send_params $my $x $y
}

# change bw
proc bicoeff::start_changebandwidth {my x} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable ${my}::previousx $x
    variable ${my}::framey1
    variable ${my}::framey2
    variable ${my}::filtercenter
    variable ${my}::lessthan_filtercenter

    if {$x < $filtercenter} {
        set lessthan_filtercenter 1
    } else {
        set lessthan_filtercenter 0
    }
    $tkcanvas bind bandedges$tag <Leave> {}
    $tkcanvas bind bandedges$tag <Enter> {}
    $tkcanvas bind bandedges$tag <Motion> {}
    $tkcanvas bind $tag <Motion> "bicoeff::band_cursor $my; bicoeff::changebandwidth $my %x"
}

proc bicoeff::changebandwidth {my x} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable ${my}::previousx
    variable ${my}::framex1
    variable ${my}::framey1
    variable ${my}::framex2
    variable ${my}::framey2
    variable ${my}::filterx1
    variable ${my}::filterx2
    variable ${my}::filterwidth
    variable ${my}::filtercenter
    variable ${my}::lessthan_filtercenter
    set dx [expr $x - $previousx]
    if {$lessthan_filtercenter} {
        if {$x < $framex1} {
            set filterx1 $framex1
            set filterx2 [expr $filterx1 + $filterwidth] 
        } elseif {$x < [expr $filtercenter - 75]} {
            set filterx1 [expr $filtercenter - 75]
            set filterx2 [expr $filtercenter + 75]
        } elseif {$x > $filtercenter} {
            set filterx1 $filtercenter
            set filterx2 $filtercenter
        } else {
            set filterx1 $x
            set filterx2 [expr $filterx2 - $dx]
        }
    } else {
        if {$x > $framex2} {
            set filterx2 $framex2
            set filterx1 [expr $filterx2 - $filterwidth] 
        } elseif {$x > [expr $filtercenter + 75]} {
            set filterx1 [expr $filtercenter - 75]
            set filterx2 [expr $filtercenter + 75]
        } elseif {$x < $filtercenter} {
            set filterx1 $filtercenter
            set filterx2 $filtercenter
        } else {
            set filterx2 $x
            set filterx1 [expr $filterx1 - $dx]
        }
    }
    set filterwidth [expr $filterx2 - $filterx1]
    set filtercenter [expr $filterx1 + ($filterwidth/2)]

    $tkcanvas coords bandleft$tag $filterx1 $framey1  $filterx1 $framey2
    $tkcanvas coords bandcenter$tag $filtercenter $framey1  $filtercenter $framey2
    $tkcanvas coords bandright$tag $filterx2 $framey1  $filterx2 $framey2
    set previousx $x

    variable ${my}::filtergain

    send_params $my $x $filtergain
}

proc bicoeff::band_cursor {my} {
    variable ${my}::tkcanvas
# cursors are set per toplevel window, not in the tkcanvas
    set mytoplevel [winfo toplevel $tkcanvas]
    $mytoplevel configure -cursor sb_h_double_arrow
}

#------------------------------------------------------------------------------#

# Tcl doesn't get the frame location from Pd in bicoeff, so we
# measure the current frame location and reset the frame x/y variables.
proc bicoeff::reset_frame_location {my} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    set coordslist [$tkcanvas coords frame$tag]
    if {[llength $coordslist] == 4} {
        variable ${my}::framex1 [lindex $coordslist 0]
        variable ${my}::framey1 [lindex $coordslist 1]
        variable ${my}::framex2 [lindex $coordslist 2]
        variable ${my}::framey2 [lindex $coordslist 3]
    }
}

proc bicoeff::enter_body {my} {
    variable ${my}::tkcanvas
    set mytoplevel [winfo toplevel $tkcanvas]
    $mytoplevel configure -cursor fleur
}

proc bicoeff::leave_body {my} {
    variable ${my}::tkcanvas
    set mytoplevel [winfo toplevel $tkcanvas]
    $mytoplevel configure -cursor $::cursor_runmode_nothing
}

proc bicoeff::enterband {my} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable markercolor
    $tkcanvas bind $tag <ButtonPress-1> {}
    $tkcanvas bind bandedges$tag <Motion> "bicoeff::band_cursor $my"
}

proc bicoeff::leaveband {my} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable markercolor
    $tkcanvas bind $tag <ButtonPress-1> "bicoeff::start_move_xy $my %x %y"
    $tkcanvas bind bandedges$tag <Motion> {}
# cursors are set per toplevel window, not in the tkcanvas
    set mytoplevel [winfo toplevel $tkcanvas]
    $mytoplevel configure -cursor fleur
}

proc bicoeff::stop_editing {my} {
    variable ${my}::tkcanvas
    variable ${my}::tag
    variable markercolor
    $tkcanvas bind $tag <Motion> {}
    $tkcanvas bind $tag <Enter> "bicoeff::enter_body $my"
    $tkcanvas bind $tag <Leave> "bicoeff::leave_body $my"
    $tkcanvas bind bandedges$tag <Enter> "bicoeff::enterband $my"
    $tkcanvas bind bandedges$tag <Leave> "bicoeff::leaveband $my"
    # cursors are set per toplevel window, not in the tkcanvas
    set mytoplevel [winfo toplevel $tkcanvas]
    $mytoplevel configure -cursor $::cursor_runmode_nothing
}

proc bicoeff::set_for_editmode {mytoplevel} {
    variable mys_in_tkcanvas
    set tkcanvas [tkcanvas_name $mytoplevel]
    if {$::editmode($mytoplevel) == 1} {
        # disable the graph interaction while editing
        if {[array names mys_in_tkcanvas -exact $tkcanvas] eq $tkcanvas} {
            foreach my $mys_in_tkcanvas($tkcanvas) {
                variable ${my}::tag
                $tkcanvas bind $tag <ButtonPress-1> {}
                $tkcanvas bind $tag <ButtonRelease-1> {}
                $tkcanvas bind bandedges$tag <ButtonPress-1> {}
                $tkcanvas bind bandedges$tag <ButtonRelease-1> {}
                $tkcanvas bind bandedges$tag <Enter> {}
                $tkcanvas bind bandedges$tag <Leave> {}
            }
        }
    } else {
        if {[array names mys_in_tkcanvas -exact $tkcanvas] eq $tkcanvas} {
            foreach my $mys_in_tkcanvas($tkcanvas) {
                variable ${my}::tag
                $tkcanvas bind $tag <ButtonPress-1> \
                    "bicoeff::start_move_xy $my %x %y"
                $tkcanvas bind $tag <ButtonRelease-1> \
                    "bicoeff::stop_editing $my"
                $tkcanvas bind bandedges$tag <ButtonPress-1> \
                    "bicoeff::start_changebandwidth $my %x"
                $tkcanvas bind bandedges$tag <ButtonRelease-1> \
                    "bicoeff::stop_editing $my"
                reset_frame_location $my
            }
        }
    }
}

#------------------------------------------------------------------------------#
# update instance before it's drawn
proc bicoeff::update {my canvas x1 y1 x2 y2} {
    variable ${my}::tkcanvas $canvas
    variable ${my}::filtergain

    # convert these all to floats so the math works properly
    variable ${my}::framex1 [expr $x1 * 1.0]
    variable ${my}::framey1 [expr $y1 * 1.0]
    variable ${my}::framex2 [expr $x2 * 1.0]
    variable ${my}::framey2 [expr $y2 * 1.0]

    variable ${my}::midpoint [expr (($framey2 - $framey1) / 2) + $framey1]
    variable ${my}::hzperpixel [expr 20000.0 / ($framex2 - $framex1)]
    variable ${my}::magnitudeperpixel [expr 0.5 / ($framey2 - $framey1)]

    variable ${my}::gainy [expr $framey1 + $filtergain]
    # TODO make these set by something else, saved state?
    variable ${my}::filterx1 [expr $framex1 + 120.0]
    variable ${my}::filterx2 [expr $framex1 + 180.0]

    variable ${my}::filterwidth [expr $filterx2 - $filterx1]
    variable ${my}::filtercenter [expr $filterx1 + ($filterwidth/2)]
}

# sets up an new instance of the class
proc bicoeff::new {my canvas bindname t x1 y1 x2 y2} {
    namespace eval $my {
        # init all here to make sure they are not blank
        variable tag "tag"             ;# unique ID for canvas elements
        variable tkcanvas ".tkcanvas"  ;# Tk canvas this is drawn on
        variable bind_name "bind_name" ;# Pd name to send callbacks to
        variable previousx 0
        variable previousy 0
        variable filtergain 0
    }
    variable ${my}::filtergain
    set filtergain [expr ($y2 - $y1) / 2]; 

    variable ${my}::bind_name $bindname
    variable ${my}::tag $t
}

proc bicoeff::drawme {my canvas name t x1 y1 x2 y2} {
    if {![namespace exists $my]} {
        new $my $canvas $name $t $x1 $y1 $x2 $y2
    }
    update $my $canvas $x1 $y1 $x2 $y2

    variable ${my}::tkcanvas
    variable ${my}::tag
    variable ${my}::framex1
    variable ${my}::framey1
    variable ${my}::framex2
    variable ${my}::framey2
    variable ${my}::filterx1
    variable ${my}::filterx2
    variable ${my}::midpoint
    variable mys_in_tkcanvas
    variable markercolor

# graph fill (gray)
    $tkcanvas create polygon $framex1 $midpoint $framex2 $midpoint \
        $framex2 $framey2 $framex1 $framey2 -fill "#dcdcdc" \
        -tags [list $tag response$tag]
# zero line/equator
    $tkcanvas create line $framex1 $midpoint $framex2 $midpoint \
        -fill $markercolor -tags [list $tag zeroline$tag]
# magnitude response graph line
    $tkcanvas create line $framex1 $midpoint $framex2 $midpoint \
        -fill "black" -width 1 \
        -tags [list $tag response$tag responseline$tag]


# left bandwidth
    $tkcanvas create line $filterx1 $framey1 $filterx1 $framey2 \
        -fill $markercolor -width 1 \
        -tags [list $tag bandleft$tag bandedges$tag]
# right bandwidth
    $tkcanvas create line $filterx2 $framey1 $filterx2 $framey2 \
        -fill $markercolor -width 1 \
        -tags [list $tag bandright$tag bandedges$tag]

# run to set things up
    stop_editing $my
#    lappend mys_in_tkcanvas($tkcanvas) $my
    if {![info exists mys_in_tkcanvas($tkcanvas)] || \
        [lsearch -exact $mys_in_tkcanvas($tkcanvas) $my] == -1} {
        lappend mys_in_tkcanvas($tkcanvas) $my
#        ::pdwindow::post "ADD TO LIST\n"
    }
    set_for_editmode [winfo toplevel $tkcanvas]
}

# sets up the class
proc bicoeff::setup {} {
    bind PatchWindow <<EditMode>> {+bicoeff::set_for_editmode %W}
}

bicoeff::setup
