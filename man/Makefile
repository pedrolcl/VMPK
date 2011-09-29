all: vmpk.1

vmpk.1: vmpk.xml
	# Verification
	xmllint --nonet --noout --postvalid --xinclude $^
	# Compilation
	xsltproc --output $@ --nonet --xinclude --xincludestyle \
	  http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl \
	  $^
