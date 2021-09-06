@ free @
expression ptr;
@@
-	free(ptr);
+	g_free(ptr);
@ malloc @
type ptr_type;
expression ptr;
expression size;
@@
-	ptr = (ptr_type) malloc(size);
+	ptr = (ptr_type) g_malloc(size);
	...
(
-	if (ptr == (ptr_type) NULL) { ... }
|
)
@ realloc @
type ptr_type;
expression ptr;
expression size;
@@
-	ptr = (ptr_type) realloc(ptr, size);
+	ptr = (ptr_type) g_realloc(ptr, size);
	...
(
-	if (ptr == (ptr_type) NULL) { ... }
|
)
