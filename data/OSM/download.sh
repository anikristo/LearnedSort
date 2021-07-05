#!/bin/bash

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/:persistentId?persistentId=doi:10.7910/DVN/JGVF9A/8FX9BV"
COMPRESSED_FILE="OSM.zst"
BINARY_FILE="OSM"

# Download the compressed file from Dataverse
echo -e "\033[32;1mDownloading the dataset...\033[0m"
curl -L ${LINK} --output ${COMPRESSED_FILE}

# Check if the user has ZSTD
if ! command -v unzstd &> /dev/null
then
    echo -e "\033[31;1mZSTD could not be found. Please install it or add it to PATH.\033[0m"
    exit
fi

# Unpack the ZST file into a binary
unzstd ${COMPRESSED_FILE} -o ${BINARY_FILE}

# Remove the zst file
rm ${COMPRESSED_FILE}

# Parse data 
echo -e "\033[32;1mParsing...\033[0m"
python3 parser.py

# Remove the binary file
rm ${BINARY_FILE}

echo -e "\033[32;1mDONE\033[0m"