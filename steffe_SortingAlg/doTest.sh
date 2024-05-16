#!/bin/bash

test=$1

case $test in
8)
# Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi
    make run NODES="2" ARGS=/mnt/raid/testlists/8Gib.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/8Gib.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/8Gib.bin;
    make run NODES="2-7,9-10" ARGS=/mnt/raid/testlists/8Gib.bin;
    ;;
12)
# Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi
    make run NODES="2" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-4" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-7" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-7,9-14" ARGS=/mnt/raid/testlists/12Gib.bin;
    ;;
30)
# Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi
    make run NODES="2" ARGS=/mnt/raid/testlists/30Gib.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/30Gib.bin;
    make run NODES="2-4" ARGS=/mnt/raid/testlists/30Gib.bin;
    make run NODES="2-6" ARGS=/mnt/raid/testlists/30Gib.bin;
    make run NODES="2-7" ARGS=/mnt/raid/testlists/30Gib.bin;
    make run NODES="2-7,9-17" ARGS=/mnt/raid/testlists/30Gib.bin;
    ;;
2481632)
# Singolo nodo che lavora sul raid
    make run NODES="2" ARGS=/mnt/raid/testlists/2Gib.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/4Gib.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/8Gib.bin;
    make run NODES="2-7,9-10" ARGS=/mnt/raid/testlists/16Gib.bin;
    make run NODES="2-7,9-18" ARGS=/mnt/raid/testlists/32Gib.bin;
    ;;
2)
# Multipli nodi che lavorano sul raid, contemporaneamente(1) su file diversi e non
    make run NODES="4" ARGS=/mnt/raid/testlists/2Gib.bin;
    ;;
24)
# Multipli nodi che lavorano sul raid, contemporaneamente(3) su file diversi e non
    make run NODES="4" ARGS=/mnt/raid/testlists/2Gib.bin;
    sleep 0.1;
    make run NODES="5-6" ARGS=/mnt/raid/testlists/4Gib.bin;
    ;;
248)
# Multipli nodi che lavorano sul raid, contemporaneamente(7) su file diversi e non
    make run NODES="4" ARGS=/mnt/raid/testlists/2Gib.bin;
    sleep 0.1;
    make run NODES="5-6" ARGS=/mnt/raid/testlists/4Gib.bin;
    sleep 0.1;
    make run NODES="1-3,7" ARGS=/mnt/raid/testlists/8Gib.bin;
    ;;
24816)
# Multipli nodi che lavorano sul raid, contemporaneamente(15) su file diversi e non
    make run NODES="4" ARGS=/mnt/raid/testlists/2Gib.bin;
    sleep 0.1;
    make run NODES="5-6" ARGS=/mnt/raid/testlists/4Gib.bin;
    sleep 0.1;
    make run NODES="1-3,7" ARGS=/mnt/raid/testlists/8Gib.bin;
    sleep 0.1;
    make run NODES="9-16" ARGS=/mnt/raid/testlists/16Gib.bin;
    ;;
#########################################################################################
0)
# Test di funzionalita su file relativamente piccolo
    make run NODES="20" ARGS=main.o;
    ;;
1)
# testing of single node speed
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
11)
# testing of 2 single nodes speed to see disk parallel availability bottleneck
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="2" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
112)
# testing of 2 single nodes speed to see disk parallel availability depends on first started
    make run NODES="2" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
111)
# testing of 3 single nodes speed to see disk parallel availability bottleneck
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="2" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="3" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
1112)
# testing of 3 single nodes speed to see disk parallel availability depends on first started
    make run NODES="2" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    sleep 0.1;
    make run NODES="3" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
124)
# testing of speed od 1-2-4 nodes doubling file size each time from 1G
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    make run NODES="1-2" ARGS=/mnt/raid/testlists/__268435456.bin;
    make run NODES="1-4" ARGS=/mnt/raid/testlists/__536870912.bin;
    ;;
1240)
# testing of speed od 1-2-4 nodes with fized size file
    make run NODES="1" ARGS=/mnt/raid/testlists/__134217728.bin;
    make run NODES="1-2" ARGS=/mnt/raid/testlists/__134217728.bin;
    make run NODES="1-4" ARGS=/mnt/raid/testlists/__134217728.bin;
    ;;
1241)
# testing of speed od 1-2-4 nodes doubling file size each time from 2G
    make run NODES="1" ARGS=/mnt/raid/testlists/__268435456.bin;
    make run NODES="1-2" ARGS=/mnt/raid/testlists/__536870912.bin;
    make run NODES="1-4" ARGS=/mnt/raid/testlists/_1073741824.bin;
    ;;
333)
# testing the merge phase with different number of files (splitting using ram 512Mb, remember to hard-code in main)
    make run NODES="2" ARGS=/mnt/raid/testlists/___67108864.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/__134217728.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/__268435456.bin;
    make run NODES="2-7" ARGS=/mnt/raid/testlists/__402653184.bin;
    make run NODES="2-7,9-10" ARGS=/mnt/raid/testlists/__536870912.bin;
    make run NODES="2-7,9-12" ARGS=/mnt/raid/testlists/__671088640.bin;
    make run NODES="2-7,9-18" ARGS=/mnt/raid/testlists/_1073741824.bin;
    ;;
666)
# testing the merge phase with different number of files (splitting using ram 1Gb, remember to hard-code in main)
    make run NODES="2" ARGS=/mnt/raid/testlists/__134217728.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/__268435456.bin;
    make run NODES="2-4" ARGS=/mnt/raid/testlists/__402653184.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/__536870912.bin;
    make run NODES="2-6" ARGS=/mnt/raid/testlists/__671088640.bin;
    make run NODES="2-7,9-10" ARGS=/mnt/raid/testlists/_1073741824.bin;
    make run NODES="2-7,9-12" ARGS=/mnt/raid/testlists/_1342177280.bin;
    make run NODES="2-7,9-14" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-7,9-18" ARGS=/mnt/raid/testlists/16Gib.bin;
    ;;
999)
# testing the merge phase with different number of files (splitting using ram 2Gb, remember to hard-code in main)
    make run NODES="2" ARGS=/mnt/raid/testlists/__268435456.bin;
    make run NODES="2-3" ARGS=/mnt/raid/testlists/__536870912.bin;
    make run NODES="2-5" ARGS=/mnt/raid/testlists/_1073741824.bin;
    make run NODES="2-6" ARGS=/mnt/raid/testlists/_1342177280.bin;
    make run NODES="2-7" ARGS=/mnt/raid/testlists/12Gib.bin;
    make run NODES="2-7,9-10" ARGS=/mnt/raid/testlists/16Gib.bin;
    make run NODES="2-7,9-17" ARGS=/mnt/raid/testlists/30Gib.bin;
    ;;
*)
    echo 'Specifica un tipo di test "doTest.sh N"';
    echo '8 --> Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi';
    echo '12 --> Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi';
    echo '30 --> Singolo nodo che lavora sul raid, stessa taglia con numero di nodi diversi';
    echo '2481632 --> Singolo nodo che lavora sul raid';
    echo '2 --> Multipli nodi che lavorano sul raid, contemporaneamente(1) su file diversi e non';
    echo '24 --> Multipli nodi che lavorano sul raid, contemporaneamente(3) su file diversi e non';
    echo '248 --> Multipli nodi che lavorano sul raid, contemporaneamente(7) su file diversi e non';
    echo '24816 --> Multipli nodi che lavorano sul raid, contemporaneamente(15) su file diversi e non';
    echo '--------------------------------'
    echo '0 --> Test di funzionalita su file relativamente piccolo';
    echo '1 --> testing of single node speed';
    echo '11 --> testing of 2 single nodes speed to see disk parallel availability bottleneck';
    echo '112 --> testing of 2 single nodes speed to see disk parallel availability depends on first started';
    echo '111 --> testing of 3 single nodes speed to see disk parallel availability bottleneck';
    echo '1112 --> testing of 3 single nodes speed to see disk parallel availability depends on first started';
    echo '124 --> testing of speed od 1-2-4 nodes doubling file size each time from 1G';
    echo '1240 --> testing of speed od 1-2-4 nodes with fized size file';
    echo '1241 --> testing of speed od 1-2-4 nodes doubling file size each time from 2G';
    echo '--------------------------------'
    echo '333 ---> testing the merge phase with different number of files (splitting using ram 512Mb, remember to hard-code in main)'
    echo '666 ---> testing the merge phase with different number of files (splitting using ram 1Gb, remember to hard-code in main)'
    echo '999 ---> testing the merge phase with different number of files (splitting using ram 2Gb, remember to hard-code in main)'
    ;;
esac
