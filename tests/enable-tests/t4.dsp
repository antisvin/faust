// faust -lang ocpp  -vec t1.dsp

import("stdfaust.lib");
myfx = dm.zita_rev1;

mute(c,fx) = _,_ <: (fx:par(i,2, (_,c:enable))), par(i,2, *(1-c)) :> _,_;

stage(i) = vgroup("stage %2i", mute(1-checkbox("mute"), myfx));

chain(n) = tgroup("fx chain", seq(i,n, stage(i)));

process = chain(4);	
