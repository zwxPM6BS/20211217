#ifndef __VIEWER_H__
#define __VIEWER_H__

#include "public.h"

int first_viewer(int sfd);
int staff_viewer(int sfd);
int admin_viewer(int sfd);
int bind_viewer(int sfd);
int modify_viewer(char info[6][30],struct mess *data_p,int grade);


#endif
