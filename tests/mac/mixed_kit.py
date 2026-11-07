"""Fixed MIDI schedules and matched gain-staging regressions for the Intel bench."""
def run_mixed(render, check, out, write_midi, custom):
    # One-shot note-offs mirror a routed pad. Alternate open/closed hat channels.
    live=[]
    for hit,frame in enumerate(range(0,350000,2720)):
        voices=[3, 6 if hit%4==0 else 5]
        if hit%2==0: voices.append(1+hit//2%2)
        if hit%4==0: voices.append(0)
        for v in voices:
            note=48 if v==0 else 48+hit%5
            vel=[127,1,100,34,121,57,88,19][hit%8]
            live.extend([(frame,144+v,note,vel),(frame+510,128+v,note,0)])
    def schedule(name,rows):
        rows=sorted(rows,key=lambda row:row[0])
        path=out/(name+'.txt')
        path.write_text(''.join('%d %d %d %d\n'%r for r in rows))
        write_midi(rows,path.with_suffix('.mid'))
        return path
    livefile=schedule('Pad-KT10-FD8',live)
    kickfile=schedule('Kick-only',[r for r in live if r[1] in (144,128)])
    def compare(name,mask,fader,slot=7,path=None,controls=None,fxmask=63,save=True):
        args=dict(mode=3,slot=slot,events_file=path,controls=controls,
                  mask=mask,fader=fader,fxmask=fxmask,save=save)
        old=render(name+'-before','previous',**args)
        new=render(name+'-after','combined',balance=True,**args)
        return old,new
    for fader in (40,70,85,100):
        for n in range(1,7):
            name=f'Mixed-{n}-{fader}pct'
            old,new=compare(name,(1<<n)-1,fader,save=fader==100 and n==1)
            check(name+' no output rails',new['full_scale']==0)
            if fader==40: check(name+' unchanged reference gain',old['sha256']==new['sha256'])
        old,new=compare(f'Live-{fader}pct',14,fader,path=livefile,controls=custom,fxmask=46)
        check(f'original kick + layered toms/snare at {fader}% no rails',new['full_scale']==0)
    old,new=compare('Isolated-kick',14,100,path=kickfile,controls=custom,fxmask=46)
    check('isolated original kick identical at full fader',old['sha256']==new['sha256'])
    no_off=schedule('Pad-no-note-offs',[r for r in live if r[1]>=144])
    with_off=render('With-note-offs','combined',3,slot=7,mask=14,fxmask=46,events_file=livefile,controls=custom)
    without=render('Without-note-offs','combined',3,slot=7,mask=14,fxmask=46,events_file=no_off,controls=custom)
    check('one-shot MIDI note-offs preserve played tails',with_off['sha256']==without['sha256'])
    stock=render('Erica-full-no-FX','original',fader=100)
    base=render('NoClip-full-no-FX','baseline',fader=100)
    bypass=render('Combined-full-no-FX','combined',mask=0,fader=100,balance=True)
    check('original mixture reaches rail without experimental DSP',stock['full_scale']>0)
    check('full-fader No Clip bypass PCM identical',base['sha256']==bypass['sha256'])
    old,new=compare('Original-kit-full',0,100)
    check('all original voices retain inherited full-fader sum',old['sha256']==new['sha256'])
    old,new=compare('Silent-layers-full',63,100,controls={1:0,2:0})
    check('zero layer levels retain original gain with live balance',old['sha256']==new['sha256'])
    edits=list(live)
    for i,param in enumerate((1,2,24,29,20,30,28,31)):
        for j,value in enumerate((110,12,80)):
            at=13000+i*34000+j*3300
            edits.extend((at,179,cc,val) for cc,val in ((99,99),(98,param),(6,value)))
    compare('Live-NRPN-edits',14,40,path=schedule('Live-NRPN',edits),controls=custom,fxmask=46)
