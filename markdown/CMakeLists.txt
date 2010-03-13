set(src amalloc.c 
        Csio.c 
        css.c 
        docheader.c 
        dumptree.c 
        generate.c 
        markdown.c 
        mkdio.c 
        resource.c 
        toc.c 
        version.c 
        xml.c 
        xmlpage.c)
        
add_library(markdown STATIC ${src})
