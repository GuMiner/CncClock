import string

# Load in the file
print ("Reading in the versioning header file...")
file = open("Version.h", "r+")
lines = file.readlines()
for index, line in enumerate(lines):
    if 'MINOR_VERSION' in line:
        splitLine = line.split(' ')
        print (splitLine)

        # Gets the last value, removes the junk, increments the version, and adds it back
        splitLine[-1] = str(int(splitLine[-1][:-2]) + 1) + ";\n"
        
        # Rewrite the line after our changes
        lines[index] = " ".join(splitLine)


# Replace the file with the new version with updated version number.
print ("Updating version number...")
file.seek(0)
for index, line in enumerate(lines):
    file.write(line)
file.truncate()

# Finish our edits
file.close()
print ("Done")
