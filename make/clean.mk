
clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f *.so  *.a
	rm -f *.0
	rm -f $(TARGET_NAME) $(LIBNAME)

distclean: clean