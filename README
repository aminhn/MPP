MPP Version 2.0 - Constraint-based Sequential Pattern Mining with Decision Diagrams

Copyright (C) 2020 Carnegie Mellon University
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

DESCRIPTION
    MPP is a constraint-based sequential pattern mining algorithm designed to mine temporal data with several attributes and hard constraints.
    	
REFERENCE
    Hosseininasab, A., van Hoeve, W.J. and Cire, A.A., 2019, July. 
    Constraint-Based Sequential Pattern Mining with Decision Diagrams. 
    In Proceedings of the AAAI Conference on Artificial Intelligence (Vol. 33, pp. 1495-1502).

CONTACT
    aminh@andrew.cmu.edu

INSTALLATION
    Designed for Linux systems. Installation from Makefile. Simply use make. Output is MPP executable.

DOWNLOAD
    MPP can be downloaded from https://github.com/aminhn/MPP

COMMAND LINE USAGE
    Example: ./MPP -file BMS.txt -thr 0.001 -att BMS_att1.txt -lg 30 -ug 900 -ls 900 - us 3600 -att BMS_att2.txt -la 30 -ua 70 -lm 40 -um 60 -out -write BMS_patt.txt

COMMAND LINE COMMANDS
	
	-file		path containing input database file
	-thr		frequency threshold in mining algorithm (double precision)
	-att		path containing input database attribute (used once per attribute)
	-lg 		lower gap constraint RHS for preceding attribute defined by -att
	-ug 		upper gap constraint RHS for preceding attribute defined by -att
	-ls 		lower span constraint RHS for preceding attribute defined by -att
	-us 		upper span constraint RHS for preceding attribute defined by -att
	-la 		lower average constraint RHS for preceding attribute defined by -att
	-ua 		upper average constraint RHS for preceding attribute defined by -att
	-lm 		lower median constraint RHS for preceding attribute defined by -att
	-um 		upper median constraint RHS for preceding attribute defined by -att
	-out		display output patterns to terminal
	-write		wrtie output patterns to file
	-dic		use if input databse events are not represented by consecutive numerical values, 
			e.g., events are represented by literals or non-consecutive numericals 
			(Code will encounter an error if not used when required. May be used in any case)

INPUT FILE 
    Input databse consists of rows of sequences, where each sequence is an ordered set of events denoted by numericals or literals. 	
    The database may be associated to a number of attribute files, where each file constains the same number of rows as the input database,
    and each row contains the same number of events per sequence of the input database. Attrbute values must be numericals.

    *** The first attribute file (generally considered to be time dimension) is assumed to satisfy a monotonic property on its values, i.e., each row is a sequence of non-decreasing numerical values.
    *** The remainder of attribute files are not required to be monotonic

OUTPUT FILE
    Ouput file, if -write command is used, gives rows of patterns with associated frequency. Frequency of a pattern is the number of sequences 
    in the input database that contain that pattern and satisfy all defined constraints on their associated attribute values.


