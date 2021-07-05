#!/bin/bash
cd data

for dataset in *; do
 echo -e "\033[34;1mDownloading the $dataset dataset...\033[0m"
 cd $dataset
 ./download.sh
 cd ..
done

cd ..
