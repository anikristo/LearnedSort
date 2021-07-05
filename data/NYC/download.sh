#!/bin/bash

echo -e "\033[32;1mDownloading the dataset...\033[0m"

## DIST.TXT
## ------------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550859"
TEXT_FILE="Dist.txt"
COMPRESSED_FILE="${TEXT_FILE}.zst"

# Download the compressed file from Dataverse
curl -L ${LINK} --output ${COMPRESSED_FILE}
echo "Downloaded ${TEXT_FILE}."

# Check if the user has ZSTD
if ! command -v unzstd &> /dev/null
then
    echo -e "\033[31;1mZSTD could not be found. Please install it or add it to PATH.\033[0m"
    exit
fi

# Unpack the ZST file into a binary
unzstd ${COMPRESSED_FILE} -o ${TEXT_FILE}

# Remove the zst file
rm ${COMPRESSED_FILE}



## PICKUP.TXT
## -------------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550793"
TEXT_FILE="Pickup.txt"
COMPRESSED_FILE="${TEXT_FILE}.zst"

# Download the compressed file from Dataverse
curl -L ${LINK} --output ${COMPRESSED_FILE}
echo "Downloaded ${TEXT_FILE}."

# Check if the user has ZSTD
if ! command -v unzstd &> /dev/null
then
    echo -e "\033[31;1mZSTD could not be found. Please install it or add it to PATH.\033[0m"
    exit
fi

# Unpack the ZST file into a binary
unzstd ${COMPRESSED_FILE} -o ${TEXT_FILE}

# Remove the zst file
rm ${COMPRESSED_FILE}



## TOT.TXT 
## ---------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550815"
TEXT_FILE="Tot.txt"
COMPRESSED_FILE="${TEXT_FILE}.zst"

# Download the compressed file from Dataverse
curl -L ${LINK} --output ${COMPRESSED_FILE}
echo "Downloaded ${TEXT_FILE}."

# Check if the user has ZSTD
if ! command -v unzstd &> /dev/null
then
    echo -e "\033[31;1mZSTD could not be found. Please install it or add it to PATH.\033[0m"
    exit
fi

# Unpack the ZST file into a binary
unzstd ${COMPRESSED_FILE} -o ${TEXT_FILE}

# Remove the zst file
rm ${COMPRESSED_FILE}


## COUNT UNIQUES AND GENERATE HISTOGRAMS
## -------------------------------------

echo -e "\033[32;1mParsing...\033[0m"
python3 parser.py

echo -e "\033[32;1mDONE\033[0m"