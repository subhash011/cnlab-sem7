#!/bin/sh
#!/bin/bash
echo "Testing FIFO......"
for R in 0.1 1.0 10.0 100.0
do
	rm -rf tempOut
	timeout 3s python3 rr.py "$R" < arrivals.txt | awk 'NF>1{print $NF}' > tempOut1
    awk 'NF>1{print $NF}' rr_"$R" > tempOut2
	out=`echo tempOut2`
	val=`diff -B -U 0 tempOut1 "$out" | grep ^@ | wc -l`
	if [ $val -eq 0 ]
	then
		echo "Rate $R: Passed"
	else
		echo "Rate $R: Failed"
	fi
done
rm -rf tempOut tempOut1 tempOut2
