# Alle5
Set of C++ functions to handle Generations 5 & 6 Pok�mon text file format

This was created to be integrated in your C++ code to let you read, modify and expand Pok�mon text text files
The workflow is really simple:
  - Use ``LoadAlle5File`` to read the text file and store each line as an entry of a ``std::vector<std::string>``
  - Add, remove or modify entries as you wish
  - Use ``SaveAlle5File`` to convert the ``std::vector<std::string>`` into a new text file

## Credits
Kaphotics - format documentation
Hello007 - command research
Brom - multi section compatibility

