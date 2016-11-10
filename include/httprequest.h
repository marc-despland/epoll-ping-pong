#ifndef _HTTPREQUEST_H
#define _HTTPREQUEST_H


class HttpRequest {
public:
	HttpRequest();
	bool finished(char * buffer,unsigned int length);
	void init();
protected:
	bool newEnd(char * buffer,unsigned int length);
	bool cr1;
	bool cr2;
	bool lf1;
	bool lf2;

};

#endif