#define MIMEMAX		32
#define MIMEMODELMAX	16
#define OBJMAX		 4

/* Object information of TMD file */
typedef struct {
    SVECTOR *vtxtop;	/* Start address of vertex information */
    u_long vtxtotal;	/* Total number of vertex information */
} TMDOBJECT;

/* Object information of MDF file (differential data file) */
typedef struct {
    SVECTOR		*top;	/* Start address of differential vector */
    u_long		offset;	/* offset of differential vector */
    u_long		total;	/* total number of differential vector */
    u_long		object;	/* object number */
} MDFOBJECT;

/* Structure of TMD information for MIMe operation  */
typedef struct {
    u_long 		objtotal;	/* Total number of objects in TMD */
    SVECTOR		*orgvtx;	/* Address of original vertex buffer */
    TMDOBJECT 	*tmdobj;	/* Address of TMD information array */
} TMDDATA;

/* Structure of MDF information for MIMe operation  */
typedef struct {
    u_long 		objtotal;	/* Total number of objects in MDF */
    MDFOBJECT	*mdfvct;	/* Address of object information array */		
    long		*mime;		/* Address of MIMe Weight (Control) array */
} MDFDATA;

void InitMime(int num, u_long *modeladdr, u_long *mdfdatavtx, u_long *orgvtxbuf);
void VertexMime(int num);

static void SetTmdInfo(u_long *model, TMDDATA *tmddt);
static void OriginalVertexSave(TMDDATA *tmddt, SVECTOR *orgvtxbf);
static void ResetMimeVertex(TMDDATA *tmddt);
static void SetMimeVertex(TMDDATA *tmddt, MDFDATA *vtxdt);
static void SetMDFVertex(u_long *mdfdtvtx, MDFDATA *vtxdt);

TMDOBJECT tmdobj0[MIMEMODELMAX][OBJMAX]; /* object information array for MIMe  */

MDFOBJECT mdfvtx0[MIMEMODELMAX][MIMEMAX]; /* differential vertex data  */

/* TMD data for MIMe */
TMDDATA tmddt0[MIMEMODELMAX];
/* differential vertex data for MIMe */
MDFDATA vtxdt0[MIMEMODELMAX];

long mimepr[MIMEMODELMAX][MIMEMAX];

void InitMime(int num, u_long *modeladdr, u_long *mdfdatavtx, u_long *orgvtxbuf)
{
	tmddt0[num].objtotal=0;
	tmddt0[num].orgvtx=(SVECTOR *)orgvtxbuf;
	tmddt0[num].tmdobj=tmdobj0[num];
	vtxdt0[num].objtotal=0;
	vtxdt0[num].mdfvct=mdfvtx0[num];
	vtxdt0[num].mime= &mimepr[num][0];

	SetTmdInfo(modeladdr, &tmddt0[num]);	/* set TMD information data */

    if (mdfdatavtx)
	{
		/* reserve original vertex data */
		OriginalVertexSave(&tmddt0[num], (SVECTOR *)orgvtxbuf);
		/* set differential vertex data */
		SetMDFVertex(mdfdatavtx,&vtxdt0[num]);
    }
}

/* set TMD information data */
static void SetTmdInfo(u_long *model, TMDDATA *tmddt)
{
    register u_long *dop;
    register int i;
    register int n;

    dop= model;
    dop++;   			/* skip file header */
    dop++;   			/* skip flag */
    n = tmddt->objtotal = *dop;  	/* total number of objects in TMD */

    dop++;
    for(i = 0; i < n; i++){
		tmddt->tmdobj[i].vtxtop = (SVECTOR *)*dop++ ;	/* start address of vertex */	
		tmddt->tmdobj[i].vtxtotal = *dop++ ;		/* total number of vertex */
    } 
}

/* set MDF data for MIMe operation (vertex) */
static void SetMDFVertex(u_long *mdfdtvtx, MDFDATA *vtxdt)
{
    register int i;
    register int n;
    register u_long *dop2;

    dop2=mdfdtvtx;
    n = vtxdt->objtotal = *dop2++;

    for(i = 0; i < n; i++){
		vtxdt->mdfvct[i].object = *dop2++;		/* object number */ 		
		vtxdt->mdfvct[i].offset = *dop2++;		/* offset of differential vertex */ 	
		vtxdt->mdfvct[i].total 	= *dop2++;		/* total number of differential vertex */ 	
		vtxdt->mdfvct[i].top 	= (SVECTOR *)dop2; 	/* address of differential information */
		dop2 	+= vtxdt->mdfvct[i].total*2;
    }
}

/* reserve original vertex data */
static void OriginalVertexSave(TMDDATA *tmddt, SVECTOR *orgvtxbf)
{
    SVECTOR *otp,*bsp;
    register int i;
    register int j;
    register int n;
    register int m;

    bsp= tmddt->orgvtx = orgvtxbf;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
		otp = tmddt->tmdobj[j].vtxtop;
		bsp += n;
		n = tmddt->tmdobj[j].vtxtotal;
			for( i = 0; i < n; i++) *(bsp+i) = *(otp+i);
    }
}

/* retrieve original vertex data */
static void ResetMimeVertex(TMDDATA *tmddt)
{
    SVECTOR *otp,*bsp;
    register int i;
    register int j;
    register int n;
    register int m;

    bsp= tmddt->orgvtx;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
		otp = tmddt->tmdobj[j].vtxtop;
		bsp += n;
		n = tmddt->tmdobj[j].vtxtotal;
			for( i = 0; i < n; i++) *(otp+i) = *(bsp+i);
    }
}

/* MIMe operation (vertex) */
static void SetMimeVertex(TMDDATA *tmddt, MDFDATA *vtxdt)
{
    SVECTOR *otp,*dfp;
    register int i;
    register int n;

    n = vtxdt->objtotal;

    for( i = 0; i < n; i++){
		otp = tmddt->tmdobj[vtxdt->mdfvct[i].object].vtxtop+vtxdt->mdfvct[i].offset;
		dfp = vtxdt->mdfvct[i].top;
		if( vtxdt->mime[i] !=0 ) gteMIMefunc(otp,dfp, vtxdt->mdfvct[i].total,vtxdt->mime[i]);  
    }
}

void VertexMime(int num)
{
	ResetMimeVertex(&tmddt0[num]);	/* retrieve original vertex */
	SetMimeVertex(&tmddt0[num],&vtxdt0[num]); /* MIMe operation (vertex) */
}

void FrameFix(int NUM,int Frame,int speed,int MaxFrame)
{
	if (mimepr[NUM][Frame]<4096)	
	{
		register int i;
		register float total = 0;

		mimepr[NUM][Frame]+=speed;

		for (i = 0; i < MaxFrame; i++)
		{
			if (i != Frame)			
			{
				if (mimepr[NUM][i] > 0)	
				{

					total+= mimepr[NUM][i];

				}
			}
		}
		
		for (i =0 ; i < MaxFrame; i++)
		{
			if (i != Frame)			
			{
				if (mimepr[NUM][i] > 0)	
				{

					register float sub = (speed/total) * mimepr[NUM][i];

					mimepr[NUM][i]-=sub;

				}
			}
			
			if	(mimepr[NUM][i] > 4096)	
			{
				mimepr[NUM][i] = 4096;
			}
			else if (mimepr[NUM][i] < 0)		
			{
				mimepr[NUM][i]=0;
			}
		}//end of for
		
		//VertexMime(NUM);
	}
}

//MIME ANIMATION
/*
void MimeAnimation()
{
	static int main_count;
	TIME_COUNTER(&main_count, 5);

	if	(main_count<8*2)	{FrameFix(0,  1,    512/2,64);}
	else if	(main_count<16*2)	{FrameFix(0,  2,    512/2,64);}
	else if	(main_count<24*2)	{FrameFix(0,  3,    512/2,64);}
	else if	(main_count<31*2)	{FrameFix(0,  0,    512/2,64);}
	if	(main_count==31*2)	{FrameFix(0,  0,     4096,64);	}
}*/