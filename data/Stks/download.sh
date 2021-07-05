#!/bin/bash

echo -e "\033[32;1mDownloading the dataset...\033[0m"

## DATE.TXT
## --------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550834"
TEXT_FILE="Date.txt"
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



## LOW.TXT
## -------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550833"
TEXT_FILE="Low.txt"
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



## OPEN.TXT 
## --------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550835"
TEXT_FILE="Open.txt"
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



## VOL.TXT 
## ----------

# Constants
LINK="https://dataverse.harvard.edu/api/access/datafile/4550832"
TEXT_FILE="Vol.txt"
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