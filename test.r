x<-read.csv("c:/curran/code/elemental/test2.csv",head=F)
x<-x[-1,]
x
rec<-as.matrix(x[1:9,])
control<-as.matrix(x[10:20,])
rec.mean<-apply(rec,2,mean)
control.mean<-apply(control,2,mean)
s.rec<-t(rec-matrix(rep(rec.mean,9),nrow=9,byrow=T))%*%(rec-matrix(rep(rec.mean,9),nrow=9,byrow=T))
s.control<-t(control-matrix(rep(control.mean,11),nrow=11,byrow=T))%*%(control-matrix(rep(control.mean,11),nrow=11,byrow=T))
sp<-(s.rec+s.control)*(1/9+1/11)/18
spinv<-solve(sp)
d<-rec.mean-control.mean
d<-matrix(d,nrow=1,byrow=T)
d%*%spinv%*%t(d)
s.control
s.rec
?history
history(max.show=1000)

x<-matrix(c(2,1,1,2),nrow=2,byrow=T)

choleski<-function(x)
{
	n<-dim(x)[1]
	p<-rep(0,n)
	
	for(i in 1:n){
		for(j in i:n){
			s<-x[i,j]
			k<-i-1
			while(k>=1){
				s<-s-x[i,k]*x[j,k]
				k<-k-1
			}
			if(i==j){
				p[i]<-sqrt(s)
			}else{
				x[j,i]<-s/p[i]
			}
		}
	}
	
	L<-x
	diag(L)<-p
	for(i in 1:n)
	{
		j<-i+1
		while(j<=n){
			L[i,j]<-0
			j<-j+1
		}
	}
	
	return(list(x=x,p=p,L=L))
}

choleski.inv<-function(x)
{
	res<-choleski(x)
	x<-res$x
	p<-res$p
	
	n<-dim(x)[1]
	for(i in 1:n){
		x[i,i]<-1/p[i]
		j<-i+1
		while(j<=n){
			s<-0
			k<-i
			while(k<=j-1)
			{
				s<-s-x[j,k]*x[k,i]
				k<-k+1
			}
			x[j,i]<-s/p[j]
			j<-j+1
		}
	}
	
	xinv<-matrix(0,n,n)
	for(i in 1:n){
		for(j in 1:n){
			for(k in max(c(i,j)):n){
				xinv[i,j]<-xinv[i,j]+x[k,i]*x[k,j]
			}
		}
	}
	return(xinv)
}

MMULT(MMULT(mean_diff,choleskiinv((1/ROWS(Control_Sample)+1/ROWS(Recovered_Sample))*(MMULT(TRANSPOSE(Control_Sample-Control_Mean),Control_Sample-Control_Mean)+MMULT(TRANSPOSE(Recovered_Sample-Recovered_Mean),Recovered_Sample-Recovered_Mean))/(ROWS(Control_Sample)+ROWS(Recovered_Sample)-2))),TRANSPOSE(mean_diff))
