#extract all these to EXTRACT CHECK
exe=/u/modiaz/UPPER_DIVISION/Fall25/CS333/CS333/labs/labs/lab2/arvik-md4
his=/u/modiaz/UPPER_DIVISION/Fall25/CS333/CS333/labs/labs/lab2/3.arvik
mine=/u/modiaz/UPPER_DIVISION/Fall25/CS333/CS333/labs/labs/lab2/chaney0.arvik
saint=/u/modiaz/UPPER_DIVISION/Fall25/CS333/CS333/labs/labs/lab2
white_devil=$saint


ccurrent_action='-c'
cargs="$ccurrent_action"
cvargs="$ccurrent_action -v"
cVargs="$ccurrent_action -V"
cEVILargs="$ccurrent_action '-v -V"

echo "into his c"
$exe $cargs $white_devil/1-s.txt $white_devil/2-s.txt $white_devil/3-s.txt > $saint/CRTS/simple3.arvik 
$exe $cvargs  $white_devil/1-s.txt $white_devil/2-s.txt $white_devil/3-s.txt > $saint/CRTS/verbose3.arvik
$exe $cVargs  $white_devil/1-s.txt $white_devil/2-s.txt $white_devil/3-s.txt > $saint/CRTS/verify3.arvik
$exe $cEVILargs  $white_devil/1-s.txt $white_devil/2-s.txt $white_devil/3-s.txt > $saint/CRTS/EVIL3.arvik

echo "into my c"
$exe $cargs  $white_devil/0-s.txt > $saint/CRTS/simple0.arvik
$exe $cvargs  $white_devil/0-s.txt > $saint/CRTS/verbose0.arvik
$exe $cVargs  $white_devil/0-s.txt > $saint/CRTS/verify0.arvik
$exe $cEVILargs $white_devil/0-s.txt > $saint/CRTS/EVIL0.arvik 

current_action='-x'
args="$current_action -f"
vargs="$current_action -v -f"
Vargs="$current_action -V -f"
EVILargs="$current_action '-v -V -f"


echo "into his x"
$exe $args $his/CRTS/simple3.arvik > $his/EXTS/simple3.txt
$exe $vargs $his/CRTS/verbose3.arvik > $his/EXTS/verbose3.txt
$exe $Vargs $his/CRTS/verify3.arvik > $his/EXTS/verify3.txt
$exe $EVILargs $his/CRTS/EVIL3.arvik > $his/EXTS/EVIL3.txt

echo "into my x"
$exe $args $his/CRTS/simple0.arvik > $his/EXTS/simple0.txt
$exe $vargs $his/CRTS/verbose0.arvik > $his/EXTS/verbose0.txt
$exe $Vargs $his/CRTS/verify0.arvik > $his/EXTS/verify0.txt
$exe $EVILargs $his/CRTS/EVIL0.arvik > $his/EXTS/EVIL0.txt


