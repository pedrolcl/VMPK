# This AWK script creates an INS (instruments definition) file from 
# an instrument list like the file "vkeybd.list" provided with vkeybd, 
# or a similar one created from scratch or with the help of the 
# sftovkb utility.
#
# usage example:
#     awk -f txt2ins.awk vkeybd.list > translated.ins

BEGIN { 
  print ".Patch Names"
  b=""
}
{ 
  if (b!=$1) { 
     print "\n[Bank" $1 "]" 
     a[$1]=1 
     b=$1
  }
  print $2 "=" $3,$4,$5,$6
}
END { 
  print "\n.Controller Names"
  print "\n[Standard]"
  print "1=1-Modulation"
  print "2=2-Breath"
  print "4=4-Foot controller"
  print "5=5-Portamento time"
  print "7=7-Volume"
  print "8=8-Balance"
  print "10=10-Pan"
  print "11=11-Expression"
  print "64=64-Pedal (sustain)"
  print "65=65-Portamento"
  print "66=66-Pedal (sostenuto)"
  print "67=67-Pedal (soft)"
  print "69=69-Hold 2"
  print "91=91-External Effects depth"
  print "92=92-Tremolo depth"
  print "93=93-Chorus depth"
  print "94=94-Celeste (detune) depth"
  print "95=95-Phaser depth"
  print "\n.Instrument Definitions"
  print "\n[" FILENAME "]"
  print "Control=Standard"
  for (i in a) { 
    print "Patch[" i "]=Bank" i 
  } 
}
