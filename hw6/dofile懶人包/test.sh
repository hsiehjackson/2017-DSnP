./cirTest  -f tests.err/do.flt > do/doflt.log
ref/./cirTest-ref -f tests.err/doref.flt > do/dofltref.log
./cirTest  -f tests.err/do.ok > do/dook.log
ref/./cirTest-ref -f tests.err/doref.ok > do/dookref.log
./cirTest  -f tests.fraig/do.sim > do/dosim.log
ref/./cirTest-ref -f tests.fraig/doref.sim > do/dosimref.log
./cirTest  -f tests.fraig/ISCAS85/do.caag > do/docaag.log
ref/./cirTest-ref -f tests.fraig/ISCAS85/doref.caag > do/docaagref.log
diff do/doflt.log do/dofltref.log 
diff do/dook.log do/dookref.log 
diff do/dosim.log do/dosimref.log 
diff do/docaag.log do/docaagref.log 
