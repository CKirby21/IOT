import './register.css';
import React from 'react';
import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';

const Register: React.FC = () => {
  return(
    <IonPage>
      <IonContent>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Register</IonTitle>
        </IonToolbar>
      </IonHeader>

        <IonList>
          <IonItem routerLink="home">
            <IonLabel>Home</IonLabel>
          </IonItem>
          <IonItem routerLink="login">
            <IonLabel>Login</IonLabel>
          </IonItem>
          <IonItem routerLink="register">
            <IonLabel>Register</IonLabel>
          </IonItem>
        </IonList>
      </IonContent>
    </IonPage>
  );
};

export default Register;