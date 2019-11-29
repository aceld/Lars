SUBDIRS = lars_reactor lars_dns lars_reporter lars_loadbalance_agent api/cpp/lars_api api/cpp/example lars_reactor/example/qps_test

.PHONY: all

all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		$(MAKE) -C $$subdir;\
	done

.PHONY: clean

clean:
	@echo Making clean
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		$(MAKE) -C $$subdir clean;\
	done
