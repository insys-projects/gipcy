
clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f *.so  *.a
	rm -f *.0
	rm -f $(TARGET_NAME) $(LIBNAME)
	rm -fv ../lib/*.so ../lib/*.so.0 ../lib/*.so.0.0 ../lib/*.ko

distclean: clean