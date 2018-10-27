import pygame
import random
import platform
import socket
import string
class stats(object):
    def __init__(self, window, winsize,font,col,sizes,SETUP):
        self.onepw = winsize[0]/100
        self.oneph = winsize[1]/100
        self.window = window
        self.font = font
        self.col = col
        self.sizes = sizes
        self.sel = [0,10]
        self.selid = [0,5]
        self.idvisible =[]
        self.itemlist = []
        self.screen = []
        self.settings = 0
        self.visible=[]
        self.redraw = False
        self.options = [[],[],[],[],["add ID","delete ID","create Tag"]]
        self.curremt = 0
        self.font2 = pygame.font.Font(SETUP['font'], int(((winsize[0]/100)*self.sizes['text_h'])//1.4))
        self.player = SETUP["player"]
        self.platform = ""
        raw = platform.platform(terse=True).split("-")
        for element in raw:
            self.platform += element+" "

        print(self.platform)
        self.ip = socket.getaddrinfo(socket.gethostname(),80)[-1:][0][-1][0]
    def update(self,select,PLAYER={}):
        if len(PLAYER)>0:
            self.player=PLAYER
        if select['knob']==0:
            pass
        if select['knob']==1:
            pass
        if select['knob']==2:
            pass
        if select['knob']==3:
            pass
        if select['knob']==4:
            if self.settings > 0:
                self.settings +=1
            if self.settings > 5000:
                self.settings = 0


            if self.settings == 0:
                f = open('gui/items.conf','r')
                self.itemlist =[]
                for line in f:
                    if line[0] != "#":
                        ln = line.split(";")
                        obj = {}
                        for elem in ln[0].split(","):
                            keyval = elem.split(":")
                            key = keyval[0]
                            val = keyval[1]
                            obj[key] = val
                        keyval = ln[1].split(":")
                        key = keyval[0]
                        val = []
                        if keyval[1]!= "NONE\n":
                            for elem in keyval[1].split(","):
                                val.append(elem.strip("\n"))
                        obj[key]=val
                        self.itemlist.append(obj)
                print(self.itemlist)


                f.close()
                self.itemlist = sorted(self.itemlist, key=lambda k: k['type'])
                self.settings+=1

            num = self.font2.render("Number of items: "+str(len(self.itemlist)),1,self.col)
            self.window.blit(num,((self.onepw*(self.sizes['end_title']-1))-num.get_width(),self.oneph*(self.sizes['start_header_h']+2)))
            self.visible = []
            itempos = [15.4,22.8,29.8,36.8,43.8,50.8,57.9,64.9,71.9,79]
            n = 0
            for items in self.itemlist:
                self.visible.append(items)
            self.screenlist = self.visible[self.sel[0]:self.sel[1]]
            # print(self.visible)
            pos = (60,66,72)
            for index,items in enumerate(self.options[select["knob"]]):
                thing = self.font.render(str(items),1,self.col)
                if index == select['selectors'][1]:
                    pygame.draw.rect(self.window,(0,50,0,1),(self.onepw*(self.sizes['end_AP']-0.5),self.oneph*(pos[index]-0.15),self.onepw*20,thing.get_height()+(self.oneph*0.5)))
                    pygame.draw.rect(self.window,self.col,(self.onepw*(self.sizes['end_AP']-0.5),self.oneph*(pos[index]-0.15),self.onepw*20,thing.get_height()+(self.oneph*0.5)),int(self.oneph*self.sizes["header_line_thick"]))
                self.window.blit(thing, (self.onepw*self.sizes['end_AP'],self.oneph*pos[index]))

            for index,items in enumerate(self.screenlist):
                thing = self.font.render(str(items['name']),1,self.col)
                if select['wheel']==index:
                    name = self.font.render("Name    :",1,self.col)
                    self.window.blit(name,(self.onepw*(self.sizes['end_title']+2),self.oneph*15))
                    self.window.blit(self.font.render(items['name'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+name.get_width(),self.oneph*15))
                    type = self.font.render("Type    :",1,self.col)
                    self.window.blit(type,(self.onepw*(self.sizes['end_title']+2),self.oneph*20))
                    self.window.blit(self.font.render(items['type'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+type.get_width(),self.oneph*20))
                    value = self.font.render("Value   :",1,self.col)
                    self.window.blit(value,(self.onepw*(self.sizes['end_title']+2),self.oneph*25))
                    self.window.blit(self.font.render(items['value'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+value.get_width(),self.oneph*25))

                    self.window.blit(self.font.render("IDLIST:",1,self.col),(self.onepw*(self.sizes['end_title']+2),self.oneph*52))
                    pygame.draw.rect(self.window, self.col, (self.onepw*(self.sizes['end_title']+1),self.oneph*59,self.onepw*29,self.oneph*30),int(self.oneph*self.sizes["header_line_thick"]))
                    self.idvisible = []
                    ipos = [60,65,70,75,80,85,90]
                    n = 0

                    self.idvisible=items['idlist']
                    self.screen = self.idvisible[self.selid[0]:self.selid[1]]
                    # print(self.idvisible)
                    # print(self.selid)

                    for ix, item in enumerate(self.screen):
                        id = self.font.render(str(item),1,self.col)
                        self.window.blit(id,(self.onepw*(self.sizes['end_title']+2),self.oneph*ipos[ix]))

                    if items['type'] == "Weapons" or items['type'] == "Apparel":
                        weight = self.font.render("Weight  :",1,self.col)
                        self.window.blit(weight,(self.onepw*(self.sizes['end_title']+2),self.oneph*30))
                        self.window.blit(self.font.render(items['weight'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+weight.get_width(),self.oneph*30))

                        if items['type'] == "Apparel":
                            dmg = self.font.render("Damred. :",1,self.col)
                            self.window.blit(dmg,(self.onepw*(self.sizes['end_title']+2),self.oneph*35))
                            self.window.blit(self.font.render(items['dmg'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+dmg.get_width(),self.oneph*35))
                            mult = self.font.render("Multiple:",1,self.col)
                            self.window.blit(mult,(self.onepw*(self.sizes['end_title']+2),self.oneph*40))
                            self.window.blit(self.font.render(items['multiple'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+mult.get_width(),self.oneph*40))

                        elif items['type'] == "Weapons":
                            damage = self.font.render("Dam     :",1,self.col)
                            self.window.blit(damage,(self.onepw*(self.sizes['end_title']+2),self.oneph*35))
                            self.window.blit(self.font.render(items['dmg'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+damage.get_width(),self.oneph*35))
                            mult = self.font.render("Multiple:",1,self.col)
                            self.window.blit(mult,(self.onepw*(self.sizes['end_title']+2),self.oneph*40))
                            self.window.blit(self.font.render(items['multiple'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+mult.get_width(),self.oneph*40))
                            ammotype = self.font.render("Ammotype:",1,self.col)
                            self.window.blit(ammotype,(self.onepw*(self.sizes['end_title']+2),self.oneph*45))
                            self.window.blit(self.font.render(items['ammotype'],1,self.col),((self.onepw*(self.sizes['end_title']+3))+ammotype.get_width(),self.oneph*45))

                    pygame.draw.rect(self.window,self.col,(self.onepw*(self.sizes['start_header_w']+1),self.oneph*(itempos[index]-0.5),(self.onepw*self.sizes['end_title'])-(self.onepw*(self.sizes['start_header_w']+1)),thing.get_height()+(self.oneph*1)),int(self.onepw*self.sizes["header_line_thick"]))
                    pygame.draw.rect(self.window,(0,50,0,1),(self.onepw*(self.sizes['start_header_w']+1),self.oneph*(itempos[index]-0.5),(self.onepw*self.sizes['end_title'])-(self.onepw*(self.sizes['start_header_w']+1)),thing.get_height()+(self.oneph*1)))
                self.window.blit(thing,(self.onepw*(self.sizes['start_header_w']+2),self.oneph*itempos[index]))
    def add(self,select,idstring=""):
        if idstring=="":
            idstring = input("enter IDs (separated by ','): ")
        if id != "exit" and idstring != "":
            print(idstring)
            idlist = idstring.split(",")
            print(idlist)
            for element in idlist:
                try:
                    self.itemlist[self.sel[0]+select['wheel']]['idlist'].index(element)
                except Exception as e:
                    if type(e)==type(ValueError()):
                        self.itemlist[self.sel[0]+select['wheel']]['idlist'].append(element)
            print(self.itemlist[self.sel[0]+select['wheel']]['idlist'])

            lines = []
            file = open('gui/items.conf','w')
            string = ""

            for element in self.itemlist:
                string=""
                for key in element:
                    if key == "idlist":
                        idlist = []
                        idlist.append(key)
                        idlist.append(element[key])
                    else:
                        string+= str(key)+":"+str(element[key])+","
                string = string[0:len(string)-1]
                string+= ";"+idlist[0]+":"
                if len(idlist[1]) > 0:
                    for elem in idlist[1]:
                        string+=str(elem)+","
                    string = string[0:len(string)-1]
                else:
                    string+="NONE"
                string+="\n"
                lines.append(string)

            for element in lines:
                file.write(element)
            file.close()
    def delete(self,select,idstring=""):
        if idstring=="":
            idstring = input("enter IDs (separated by ','): ")
        if idstring != "exit" and id != "":
            idlist = idstring.split(",")

            print(idlist)
            for element in idlist:
                try:

                    index = self.itemlist[self.sel[0]+select['wheel']]['idlist'].remove(str(element))
                    print(self.itemlist[index]["idlist"])
                except Exception as e:
                    print("error")
                    if type(e)==type(ValueError()):
                        print("wrong ID")
            print(self.itemlist[self.sel[0]+select['wheel']]['idlist'])

            lines = []
            file = open('gui/items.conf','w')
            string = ""

            for element in self.itemlist:
                string=""
                for key in element:
                    if key == "idlist":
                        idlist = []
                        idlist.append(key)
                        idlist.append(element[key])
                    else:
                        string+= str(key)+":"+str(element[key])+","
                string = string[0:len(string)-1]
                string+= ";"+idlist[0]+":"
                if len(idlist[1]) > 0:
                    for elem in idlist[1]:
                        string+=str(elem)+","
                    string = string[0:len(string)-1]
                else:
                    string+="NONE"
                string+="\n"
                lines.append(string)

            for element in lines:
                file.write(element)
            file.close()
    def generateID(self):
        output = ''.join(random.choice(string.ascii_uppercase+string.digits+string.ascii_lowercase) for i in range(12))
        return output
