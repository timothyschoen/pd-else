# generate menu tree for native objects for the canvas right click popup
# code by Porres and Seb Shader

package require pd_menus

namespace eval category_else_menu {
}

proc menu_send_else_obj {w x y item} {
    if {$item eq "else"} {  
        pdsend "$w obj $x $y $item"
    } else {
        pdsend "$w obj $x $y else/$item"
        set abslist {abs.pd~ allpass.filt~ float2imp~ batch.rec~ batch.write~ bin.shift~ bl.osc~ bl.wavetable~ blip~ bpbank~ brickwall~ chorus~ compress~ crusher~ dir damp.osc~ dispatch drunkard~ duck~ echo.rev~ envelope~ expand~ flanger~ float2sig~ free.rev~ freeze~ gain~ gain2~ level~ gatehold gatehold~ gaterelease gatedelay gaterelease~ grain.live~ grain.sampler~ grain.synth~ gran.player~ graph~ hann~ hip.bw~ lop.bw~ mag~ maxpeak~ meter~ meter2~ meter4~ meter8~ metronome~ mix2~ mix4~ mono.rev~ morph~ ms2samps~ noisegate~ norm~ oscbank~ oscnoise~ out.mc~ out~ out4~ out8~ perlin~ phaser~ ping.pong~ pitch.shift~ plate.rev~ play.file~ player~ polymetro~ pvoc.freeze~ pvoc.live~ pvoc.player~ rec.file~ resonbank~ resonbank2~ revdelay~ rm~ sample~ samps2ms~ setdsp~ spectrograph~ stereo.rev~ stretch.shift~ synth~ tremolo~ trig2bang~ vibrato~ vocoder~ voices~ above add amean any2symbol autotune autotune2 avg bangdiv biplot bpm brown car2pol cents2frac cents2scale chrono circle clock coeff2pz combine count db2lin dec2frac dec2hex deg2rad delete display drum.seq drunkard e eqdiv equal euclid frac.add frac.mul frac2cents frac2dec freq2midi glide glide2 smooth smooth2 gmean group hex2dec histogram impulse insert interpolate iterate keymap keypress lastvalue lcm lfnoise lfo lin2db list.inc list.seq mag makenote2 markov median meter midi.clock midi.in midi.learn midi.out midi2freq mono mono~ morph mov.avg ms2samps mtx.ctl multi.vsl nmess notedur2ratio note2midi op osc.receive osc.send pattern phasor pi pick pimp midi2note pol2car polymetro presets pulse pz2coeff rad2deg rampnoise rand.dist rand.list randpulse randpulse2 range.hsl range rec2 remove replace retune reverse rotate samps2ms scala scale2cents scale2freq scales schmitt score score2 scramble sendmidi sequencer slew slew2 slider2d speed stack status stepnoise store stream sum swap2 sysrt.in sysrt.out tabgen tap tempo timed.gate trig2bang unite zbiplot}
        foreach abstraction $abslist {
            if {$item eq $abstraction} {  
                pdsend "pd-$item.pd loadbang"
                break
            }
        }
    } 
}


# set nested list
proc category_else_menu::load_menutree {} {
    set menutree { 
        {else
            {assorted
                {else}}
            {gui
                {knob numbox~ drum.seq bicoeff pad messbox mtx.ctl biplot zbiplot pic colors function circle slider2d display out.mc~ out~ out4~ out8~ gain~ gain2~ level~ button popmenu keyboard graph~ range.hsl multi.vsl spectrograph~ meter~ meter2~ meter4~ meter8~ note mix2~ mix4~ setdsp~ openfile scope~ scope3d~}}
            {time
                {chrono datetime}}
            {fft
                {hann~ bin.shift~}}
            {table
                {buffer tabgen tabreader tabreader~}}
            {tuning/notes
                {scales scale2freq scala autotune autotune2 makenote2 retune eqdiv cents2scale scale2cents cents2frac frac2cents dec2frac frac2dec freq2midi midi2freq note2midi midi2note notedur2ratio}}
            {patch/subpatch\ management
                {loadbanger args meter presets dollsym sender receiver retrieve dispatch var send2~ blocksize~ nop~ click properties fontsize canvas.active canvas.bounds canvas.gop canvas.pos canvas.edit canvas.vis canvas.name canvas.setname canvas.zoom sendmidi abs.pd~}}
            {message\ management
                {format swap2 nmess unite separate symbol2any any2symbol changed hot initmess message default pack2 pick limit spread router route2 routeall routetype selector stack store morph interpolate sig2float~ float2sig~ pipe2}}
            {list\ management
                {break order combine delete remove equal group iterate insert scramble sort reverse rotate replace sum slice stream merge unmerge amean gmean list.inc lace delace}}
            {file\ management
                {dir}}
            {midi
                {midi midi.learn midi.in midi.out sysrt.in sysrt.out ctl.in ctl.out touch.in touch.out ptouch.in ptouch.out pgm.in pgm.out bend.in bend.out note.in note.out midi.clock noteinfo panic mono mono~ voices voices~ suspedal}}
            {osc/network
                {osc.route osc.format osc.parse osc.send osc.receive pdlink pdlink~}}
            {math\ functions
                {add add~ median avg mov.avg count gcd lcm frac.add frac.mul ceil ceil~ factor floor floor~ trun trunc~ rint rint~ quantizer quantizer~ fold fold~ lastvalue mag mag~ sin~ tanh~ wrap2 wrap2~ op op~}}
            {math\ conversion
                {hex2dec dec2hex bpm car2pol car2pol~ cents2ratio cents2ratio~ ms2samps ms2samps~ db2lin db2lin~ dbgain~ float2bits hz2rad lin2db lin2db~ deg2rad rad2deg pz2coeff coeff2pz rad2hz ratio2cents ratio2cents~ samps2ms samps2ms~ pol2car pol2car~ rescale rescale~}}
            {math\ constant\ values
                {sr~ nyquist~ pi e}}
            {logic
                {loop}}
            {audio\ multichannel\ tools
                {nchs~ sigs~ repeat~ select~ pick~ get~ sum~ merge~ unmerge~ slice~ lace~ delace~}}       
            {analog\ circuitry\ emulation
                {circuit~}}    
            {scripting
                {lua}}
            {fx\ assorted
                {downsample~ conv~ chorus~ shaper~ crusher~ drive~ power~ flanger~ freq.shift~ pitch.shift~ ping.pong~ rm~ tremolo~ vibrato~ vocoder~ morph~ freeze~ pvoc.freeze~ phaser~}}
            {fx\ delay
                {del~ in~ del~\ out fbdelay~ ffdelay~ revdelay~ filterdelay~}}
            {fx\ dynamics
                {compress~ duck~ expand~ noisegate~ norm~}}
            {fx\ reverberation
                {allpass.rev~ comb.rev~ echo.rev~ mono.rev~ stereo.rev~ free.rev~ giga.rev~ plate.rev~ fdn.rev~}}
            {fx\ filters
                {allpass.2nd~ allpass.filt~ bitnormal~ comb.filt~ lop.bw~ hip.bw~ biquads~ bandpass~ bandstop~ crossover~ bpbank~ bicoeff2 brickwall~ eq~ highpass~ highshelf~ lop2~ lowpass~ lowshelf~ mov.avg~ resonbank~ resonbank2~ resonant~ resonator~ resonator2~ svfilter~}}
            {sampling\ playing\ granulation
                {player~ gran.player~ stretch.shift~ pvoc.player~ pvoc.live~ batch.rec~ bach.write~ rec.file~ play.file~ tabplayer~ tabwriter~ sfload sfinfo sample~}}
            {synthesis:\ synthesizers
                {pm2~ pm4~ pm6~ sfont~ sfz~ plaits~ synth~}}
            {synthesis:\ granular
                {grain.synth~ grain.live~ grain.sampler~}}
            {synthesis:\ physical\ modeling
                {pluck~}}
            {synthesis:\ oscillators
                {cosine~ impulse~ impulse2~ parabolic~ pulse~ saw~ saw2~ oscbank~ oscbank2~ oscnoise~ sine~ square~ tri~ gaussian~ vsaw~ fm~ pm~ wavetable~ wt2d~ blip~ bl.osc~ bl.imp~ bl.imp2~ bl.saw~ bl.saw2~ bl.square~ bl.tri~ bl.vsaw~ bl.wavetable~ damp.osc~}}
            {synthesis:\ chaotic\ stochastic\ noise
                {white~ brown~ perlin~ velvet~ crackle~ cusp~ fbsine~ fbsine2~ gbman~ gray~ henon~ ikeda~ latoocarfian~ lorenz~ lfnoise~ lincong~ logistic~ quad~ stepnoise~ rampnoise~ randpulse~ randpulse2~ standard~ pink~ xmod~ xmod2~ gendyn~}}
            {control:\ mouse\ keyboard
                {mouse canvas.mouse keycode keymap keypress}}
            {control:\ fade\ pan\ routing
                {fader~ autofade~ autofade.mc~ autofade2~ autofade2.mc~ balance~ pan~ pan.mc~ pan2~ pan4~ spread~ spread.mc~ rotate~ rotate.mc~ xfade~ xfade.mc~ xgate~ xgate.mc~ xgate2~ xgate2.mc~ xselect~ xselect.mc~ xselect2~ xselect2.mc~ mtx~ mtx.mc~}}
            {control:\ sequencers
                {euclid score score2 pattern list.seq sequencer sequencer~ phaseseq~ impseq~ rec rec2}}
            {control:\ envelopes
                {decay~ asr~ adsr~ envgen~ envelope~}}
            {control:\ ramp\ line/curve\ generators
                {ramp~ susloop~ function~ slew slew~ slew2 slew2~ lag~ lag2~ glide glide~ glide2 glide2~ smooth~ smooth smooth2~ smooth2}}
            {control:\ random/stochastic
                {rand.f rand.f~ rand.i rand.i~ rand.list rand.u rand.dist rand.hist histogram markov drunkard drunkard~ brown randpulse randpulse2 lfnoise stepnoise rampnoise}}
            {control:\ control\ rate\ lfo
                {lfo phasor pimp impulse pulse}}
            {control:\ triggers
                {above above~ bangdiv chance chance~ dust~ dust2~ gatehold gatehold~ gaterelease gaterelease~ gatedelay~ gatedelay gate2imp~ pimp~ pimpmul~ pulsecount~ pulsediv~ sh~ schmitt schmitt~ status status~ trig.delay~ trig.delay2~ toggleff~ timed.gate timed.gate~ match~ trig2bang trig2bang~ trighold~}}
            {control:\ triggers\ clock
                {clock metronome metronome~ polymetro polymetro~ speed tempo tempo~}}
            {analysis
                {changed~ changed2~ detect~ lastvalue~ median~ peak~ tap range range~ maxpeak~ rms~ mov.rms~ vu~ zerocross~ beat~}}
        }
    }
    return $menutree
}

proc category_else_menu::create {cmdstring code result op} {
    set mymenu [lindex $cmdstring 1]
    set x [lindex $cmdstring 3]
    set y [lindex $cmdstring 4]
    set menutree [load_menutree]
    $mymenu add separator
    foreach categorylist $menutree {
        set category [lindex $categorylist 0]
        menu $mymenu.$category
        $mymenu add cascade -label $category -menu $mymenu.$category
        foreach subcategorylist [lrange $categorylist 1 end] {
            set subcategory [lindex $subcategorylist 0]
            menu $mymenu.$category.$subcategory
            $mymenu.$category add cascade -label $subcategory -menu $mymenu.$category.$subcategory
            foreach item [lindex $subcategorylist end] {
                # replace the normal dash with a Unicode minus so that Tcl does not
                # interpret the dash in the -label to make it a separator
                $mymenu.$category.$subcategory add command \
                    -label [regsub -all {^\-$} $item {−}] \
                    -command "menu_send_else_obj \$::focused_window $x $y {$item}"
            }
        }
    }
}

trace add execution ::pdtk_canvas::create_popup leave category_else_menu::create
