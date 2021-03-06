***********************************************************************
TOOL:  SVMTool++
AUTHOR(s): Jesus Gimenez, Lluis Marquez and Senen Moya
CONTRIBUTIONS: Gael de Chalendar <Gael.de-Chalendar@cea.fr>
DATE:   23/02/2007
VERSION: 1.1.6
DESCRIPTION: A general POS tagger generator based on Support Vector Machines.
***********************************************************************

Contents
--------

LGPL.txt   _ license terms.
README.TXT _ this file.
CMakeLists.txt   _ main cmake file to compile and link SVMTool
     ./sample _ API sample.
        * main.cc    _ Sample source
        * main       _ Sample program
     ./src    _ SVMTool sources.

Compilation products
--------------------
* libsvmtool.so _ The SVMTool library, used by the executables and usable from
  you own programs
* SVMTagger  _ POS-tagger
* SVMTeval   _ evaluation component
* SVMTlearn  _ learning component
               
Description
-----------

SVMT is a very simple and effective part-of-speech tagger based on Support Vector Machines. By means of a rigorous experimental evaluation, we conclude that the proposed SVM-based tagger is robust and flexible for feature modelling (including lexicalization), trains efficiently with almost no parameters to tune, and is able to tag thousands of words per second, which makes it really practical for real NLP applications. Regarding accuracy, the SVM-based tagger significantly outperforms the TnT tagger exactly under the same conditions, and achieves a very competitive accuracy of 97.17% for English on the WSJ corpus, which is comparable to the best taggers reported up to date. This prototype is implemented in C++. 

The SVMlight software implementation of Vapnik's Support Vector Machine [Vapnik, 1995] by Thorsten Joachims has been used to train the models. For further information on it see:

    * T. Joachims, Making large-Scale SVM Learning Practical. Advances in Kernel Methods - Support Vector Learning, B. Schölkopf and C. Burges and A. Smola (ed.), MIT-Press, 1999.



COMPONENTS:
===========

The SVMTool consists of three main components:

    * SVMTlearn
    * SVMTagger
    * SVMTeval

These are, namely the learner, tagger and evaluator.

---------------------------------------------------------------------
(1) SVMTlearn 
---------------------------------------------------------------------

Given a training set of examples (either annotated or unannotated), it is responsible for the training of a set of SVM classifiers. So as to do that, it makes use of SVM-light software package, an implementation of Vapnik's SVMs in C, developed by Thorsten Joachims.

Training data must be columned, i.e. a token per line corpus in a sentece by sentence fashion. The token is expected to be the first field of the line. The POS takes the second field in the output. The rest of the line is ignored.

SVMTlearn behaviour is easily adjusted through a configuration file. These are the currently available options:
	- Sliding window (size and core position)
	- Feature set (word features, POS features, ortographic features)
	- Feature filtering (count cutoff and max mapping size)
	- SVM model compression
	- C parameter tunning
        - Test [ against a test set or via cross-validation ]
	- Dictionary repairing (either heuristically and/or based on a correction list)
	- Ambiguous classes (may be optionally provided)
	- Open classes (may be optionally provided)
	- Backup lexicon (may be optionally provided)


---------------------------------------------------------------------
(2) SVMTagger
---------------------------------------------------------------------

Given a text corpus (one token per line) and the path to a previously learned SVM model (including the automatically generated dictionary), it performs the POS tagging of a sequence of words. The tagging goes on-line based on a sliding window which gives a view of the feature context to be considered at every decision. Calculated part-of-speech tags feed directly forward next tagging decisions as context features.

The SVMtagger component works on standard input/output. It processes a token per line corpus in a sentence by sentence fashion. The token is expected to be the first field of the line. The predicted POS will take the second field in the output. The rest of the line remains unchanged. Lines beginning with '##' are just ignored by the tagger.

If you integrate SVMTool in a larger application that does its own morphologic analysis, you can pass the possible tags computed by your tools, thus helping SVMTagger to handle unknown words (for example named entities or words with an unusual case). In this case, the input format will take a second element on each line: the parenthesized, comma-separated list of possible tags. For example:
token1 (tag1,tag2)
token2 (tag3,tag1,tag4)
...

SVMTagger is very flexible, and adapts very well to the needs of the user. Thus you may find the several options currently available:

	- Tagging scheme (greedy/sentence-level)
	- Tagging direction (left-to-right, right-to-left, or both)
	- One pass / Two passes
	- SVM Model Compression
        - Get all predicitons (not only the winner)
        - Use of a softmax function to transform predictions into probabilities
	- Backup lexicon (may be optionally provided)


---------------------------------------------------------------------
(3) SVMTeval
---------------------------------------------------------------------

Given a SVMTool predicted tagging output and the corresponding gold-standard, SVMTeval evaluates the performance in terms of accuracy. It is a very useful component for the tunning of the system parameters, such as the C parameter, the feature patterns and filtering, the model compression et cetera.

Moreover, based on a given morphological dictionary (e.g. the automatically generated at training time) results may be presented also for different sets of words (known words vs unknown words, ambiguous words vs unambiguous words).  A different view of these same results can be seen from the class of ambiguity perspective, too, i.e., words sharing the same kind of ambiguity may be considered together. Also words sharing the same degree of disambiguation complexity, determined by the size of their ambiguity classes, can be
grouped.

	- mode:
		* 0 : full report
		* 1 : brief report (overall accuracy) 
		* 2 : comparing known vs unknown words
		* 3 : grouping words according to their level of ambiguity
		* 4 : grouping words according to their class of ambiguity
		* 5 : from the part-of-speech point of view


---------------------------------------------------------------------


NOTES:
======
- comments from nlp community members are very welcome.

- Perl version is available at http://www.lsi.upc.edu/~SVMTool

- If you're interested in getting oncoming updates of this freely available software please do not hesitate to e-mail us:
	* Jesus Gimenez, at jgimenez@lsi.upc.edu
	* Lluis Marquez, at lluism@lsi.upc.edu


CONTRIBUTING:
=============

The SVMTool library is licensed under LGPL , which means that it may be linked to and used by commercial software packages. But the license also enforces that any changes or improvements made to the library (and in this case also to the morphological data) must be redistributed under LGPL terms.

Thus, if you improve the software or data, either adding new functionalities, fixing bugs, or adding analyzers for new languages, you can not distribute them under different conditions than those stated in the license (i.e. freely and with no usage restrictions).

If you want that your changes and improvements become useful to many other people using this free software, please contact us ( jgimenez@lsi.upc.es ).


SVMTool Discussion Group
========================

Discussion on features and bugs of this software as well as information
about oncoming updates takes place on the SVMTool group, to which
you can subscribe at:

http://groups-beta.google.com/group/SVMT

and post messages at:

SVMT@googlegroups.com


REFERENCES:
==========

Please reference this tool in your academic works citing the following paper:

* Jesus Gimenez and Lluis Marquez
  SVMTool: A general POS tagger generator based on Support Vector Machines.
  Proceedings of the 4th International Conference on Language Resources and Evaluation (LREC'04).
  Lisbon, Portugal. 2004.
