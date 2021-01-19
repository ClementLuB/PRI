#include "ci2c.h"

CI2c::CI2c(QObject *parent, char noBus) :
    QObject(parent)
{    
    mNoBus = noBus;
    mNbLink=0;
} // constructeur

CI2c * CI2c::mSingleton = NULL;

intCI2c::lire(unsignedchar addr,  unsignedchar*buffer, int lg)
{
    if(ioctl(mFileI2c, I2C_SLAVE, addr)!=0) 
    {  // Règle le driver I2Csur l'adresse.
        qDebug("Erreur ioctl acces au bus I2C");
        return -1;
     } // if ioctl
      bzero(buffer, lg);
      QMutexLocker lock(&this->mutexI2c);  // verrouillage du mutex. Ilest libéré en sortie de méthode
      
      int nb=read(mFileI2c, buffer, lg);
//    qDebug() << "CI2c:lire: " << buffer[0] << " " << buffer[1] << buffer[2] << " " << buffer[3] << buffer[4] << " " << buffer[5];
      return nb;
} // lire

intCI2c::ecrire(unsignedchar addr, unsignedchar*buffer, int lg)
{
    if(ioctl(mFileI2c, I2C_SLAVE, addr)!=0) 
    {  // Règle le driver I2Csur l'adresse.
        qDebug("Erreur ioctl acces au bus I2C");
        return -1;
    }// if ioctl
    QMutexLocker lock(&this->mutexI2c);  // verrouillage du mutex. Ilest libéré en sortie de méthode
    
 int nb=write(mFileI2c, buffer, lg);
 
 //    qDebug() << "CI2c:ecrire: nb=" << nb << " : " << buffer[0] << "" << buffer[1] << buffer[2];
    return nb;
 } // ecrire
 intCI2c::init()
 {
     char filename[20];
     sprintf(filename, "/dev/i2c-%c",mNoBus);
     if((mFileI2c=open(filename, O_RDWR))==-1) 
     {  // ouvre le fichier virtuel d'accès à l'I2C
         qDebug("Erreur ouverture acces au bus I2C");
         return -1;
      } // if open
      return mFileI2c;
 } // init
 
 intCI2c::getNbLink()
 {
    return mNbLink;
 } // getNbLink
 
 CI2c *CI2c::getInstance(QObject *parent, char no)
 {
    if (mSingleton == NULL)
    {
       qDebug("L'objet CI2c sera créé !");
       mSingleton =  new CI2c(parent, no);
       mSingleton->init();
       mSingleton->mNbLink=1;
    }
    else
    {
         mSingleton->mNbLink++;
         qDebug("singleton already created!");
     }
      return mSingleton;
 } // getInstance
 
 voidCI2c::freeInstance()
 {
    if (mSingleton != NULL)
    {
        mSingleton->mNbLink--;
        if (mSingleton->mNbLink==0)
        {
           close(mSingleton->mFileI2c);
           delete mSingleton;
           mSingleton = NULL;
        } // if mNbLink
    } // if null
 } // freeInstance
